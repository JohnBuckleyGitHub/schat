/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore>
#include <QtNetwork>

#include "clientservice.h"
#include "packet.h"
#include "schat.h"

static const int CheckTimeout         = 12000;
static const int ReconnectTimeout     = 4000;

/*!
 * \brief Конструктор класса ClientService.
 */
ClientService::ClientService(AbstractProfile *profile, const Network *network, QObject *parent)
  : AbstractPeer(parent),
  m_profile(profile),
  m_accepted(false),
  m_fatal(false),
  m_network(network),
  m_reconnects(0),
  m_safeNick(profile->nick())
{
  m_stream.setVersion(StreamVersion);
  m_checkTimer.setInterval(CheckTimeout);
  m_ping.setInterval(22000);
  m_reconnectTimer.setInterval(ReconnectTimeout);

  connect(&m_checkTimer, SIGNAL(timeout()), SLOT(check()));
  connect(&m_reconnectTimer, SIGNAL(timeout()), SLOT(reconnect()));
  connect(&m_ping, SIGNAL(timeout()), SLOT(ping()));
}


ClientService::~ClientService()
{
  SCHAT_DEBUG(this << "::~ClientService()")
}


/*!
 * Отправка пакета `OpcodeMessage` на сервер, ник отправителя находится в удалённом сервисе.
 * const QString &channel -> канал/ник для кого предназначено сообщение (пустая строка - главный канал).
 * const QString &message -> сообщение.
 * ----
 * Возвращает `true` в случае успешной отправки (без подтверждения сервером).
 */
bool ClientService::sendMessage(const QString &channel, const QString &message)
{
  SCHAT_DEBUG(this << "::sendMessage(const QString &, const QString &)" << channel << message)

  PacketBuilder builder(Packet::Message);
  builder.add(Packet::UTF16, channel);
  builder.add(Packet::UTF16, message);

  return send(builder);
}


bool ClientService::sendRelayMessage(const QString &channel, const QString &sender, const QString &message)
{
  SCHAT_DEBUG(this << "::sendRelayMessage(const QString &, const QString &, const QString &, quint8)" << channel << sender << message)

  PacketBuilder builder(OpcodeRelayMessage);
  builder.add(Packet::UTF16, channel);
  builder.add(Packet::UTF16, sender);
  builder.add(Packet::UTF16, message);

  return send(builder);
}


/*!
 * Отправка универсального пакета.
 *
 * \param sub   Субопкод.
 * \param data1 Список данных типа quint32
 * \param data2 Список данных типа QString
 */
bool ClientService::sendUniversal(quint16 sub, const QList<quint32> &data1, const QStringList &data2)
{
  PacketBuilder builder(OpcodeUniversal);
  builder.add(Packet::UINT16, sub);
  builder.add(data1);
  builder.add(data2);

  return send(builder);
}


void ClientService::quit(bool end)
{
  SCHAT_DEBUG(this << "::quit(bool)" << end);

  if (m_socket) {
    if (m_socket->state() == QTcpSocket::ConnectedState) {
      m_fatal = end;
      m_socket->disconnectFromHost();
    }
    else {
      m_socket->deleteLater();
      m_socket = 0;
    }
  }

  m_fatal = end;
  if (end) {
    emit unconnected(false);
    emit fatal();
    m_checkTimer.stop();
    m_reconnectTimer.stop();
  }
}


void ClientService::sendByeMsg(const QString &msg)
{
  PacketBuilder builder(OpcodeByeMsg);
  builder.add(Packet::UTF16, msg);

  send(builder);
}


void ClientService::sendNewUser(const QStringList &list, quint8 echo, quint8 numeric)
{
  PacketBuilder builder(OpcodeNewUser);
  builder.add(Packet::UINT8, echo);
  builder.add(Packet::UINT8, numeric);
  builder.add(Packet::UINT8, AbstractProfile::genderNum(list.at(AbstractProfile::Gender)));
  builder.add(Packet::UTF16, list.at(AbstractProfile::Nick));
  builder.add(Packet::UTF16, list.at(AbstractProfile::FullName));
  builder.add(Packet::UTF16, list.at(AbstractProfile::ByeMsg));
  builder.add(Packet::UTF16, list.at(AbstractProfile::UserAgent));
  builder.add(Packet::UTF16, list.at(AbstractProfile::Host));

  send(builder);
}


void ClientService::sendSyncBye(const QString &nick, const QString &bye)
{
  PacketBuilder builder(OpcodeSyncByeMsg);
  builder.add(Packet::UTF16, nick);
  builder.add(Packet::UTF16, bye);

  send(builder);
}


void ClientService::sendSyncProfile(quint8 gender, const QString &nick, const QString &nNick, const QString &name)
{
  PacketBuilder builder(OpcodeNewNick);
  builder.add(Packet::UINT8, gender);
  builder.add(Packet::UTF16, nick);
  builder.add(Packet::UTF16, nNick);
  builder.add(Packet::UTF16, name);

  send(builder);
}


void ClientService::sendUserLeave(const QString &nick, const QString &bye, quint8 flag)
{
  PacketBuilder builder(OpcodeUserLeave);
  builder.add(Packet::UINT8, flag);
  builder.add(Packet::UTF16, nick);
  builder.add(Packet::UTF16, bye);

  send(builder);
}


/*!
 * Подключение к хосту, за выдачу адреса сервера и порта отвечает класс `m_network`.
 * В случае попытки подключения высылается сигнал `void connecting(const QString &, bool)`.
 */
void ClientService::connectToHost()
{
  SCHAT_DEBUG(this << "::connectToHost()")
  if (m_socket) {
    SCHAT_DEBUG(m_socket->state())
  }

  if (!m_socket)
    createSocket();

  m_fatal = false;

  if (m_socket->state() == QAbstractSocket::UnconnectedState) {
    m_server = m_network->server();
    if (m_server.address == "127.0.0.1" || m_server.address == "localhost" || activeInterfaces())
      m_socket->connectToHost(m_server.address, m_server.port);

    if (m_network->isNetwork())
      emit connecting(m_network->name(), true);
    else
      emit connecting(m_server.address, false);

    m_checkTimer.start();
  }
  else if (m_socket->state() == QAbstractSocket::ConnectedState) {
    m_reconnects = 0;
    m_socket->disconnectFromHost();
  }
}


void ClientService::sendNewProfile()
{
  PacketBuilder builder(OpcodeNewProfile);
  builder.add(Packet::UINT8, m_profile->genderNum());
  builder.add(Packet::UTF16, m_profile->nick());
  builder.add(Packet::UTF16, m_profile->fullName());

  send(builder);
}


/*!
 * \todo Добавить поддержку склеенных пакетов.
 */
void ClientService::readPacket(int pcode, const QByteArray &block)
{
  AbstractPeer::readPacket(pcode, block);

  PacketReader reader(pcode, block);

  if (m_accepted) {
    switch (pcode) {
      case OpcodeNewUser:
        opcodeNewUser(reader);
        break;

      case Packet::Message:
        messagePacket(reader);
        break;

      case OpcodeUserLeave:
        opcodeUserLeave(reader);
        break;

      case OpcodePrivateMessage:
        opcodePrivateMessage(reader);
        break;

      case OpcodePing:
        opcodePing();
        break;

      case OpcodeNewProfile:
        opcodeNewProfile(reader);
        break;

      case OpcodeNewNick:
        opcodeNewNick(reader);
        break;

      case OpcodeServerMessage:
        opcodeServerMessage(reader);
        break;

      case OpcodeNewLink:
        opcodeNewLink(reader);
        break;

      case OpcodeLinkLeave:
        opcodeLinkLeave(reader);
        break;

      case OpcodeRelayMessage:
        opcodeRelayMessage(reader);
        break;

      case OpcodeSyncNumerics:
        opcodeSyncNumerics(reader);
        break;

      case OpcodeSyncUsersEnd:
        emit syncUsersEnd();
        break;

      case OpcodeSyncByeMsg:
        opcodeSyncByeMsg(reader);
        break;

      case OpcodeUniversal:
        opcodeUniversal(reader);
        break;

      default:
        break;
    }
  }
  else if (pcode == OpcodeAccessGranted) {
    opcodeAccessGranted(reader);
  }
  else if (pcode == OpcodeAccessDenied) {
    opcodeAccessDenied(reader);
  }
  else {
    m_socket->disconnectFromHost();
    return;
  }


}


/*!
 * Разрыв соединения или переподключение если после `CheckTimeout` миллисекунд не удалось установить действующие соединение.
 */
void ClientService::check()
{
  SCHAT_DEBUG(this << "::check()")

  if (m_socket) {
    if (m_socket->state() != QTcpSocket::ConnectedState) {
      m_socket->deleteLater();
      m_socket = 0;
      connectToHost();
    }
    else if (!m_accepted && m_socket->state() == QTcpSocket::ConnectedState)
      m_socket->disconnectFromHost();
    else
      m_checkTimer.stop();
  }
  else
    m_checkTimer.stop();
}


/*!
 * Слот вызывается при успешном подключении сокета `m_socket` к серверу.
 * Слот отправляет приветственное сообщение серверу (OpcodeGreeting).
 * Таймер переподключения `m_reconnectTimer` отстанавливается.
 */
void ClientService::connected()
{
  SCHAT_DEBUG(this << "::connected()" << m_profile->nick())

  m_nextBlockSize = 0;
  m_reconnectTimer.stop();

  PacketBuilder builder(Packet::HandshakeRequest);
  builder.add(Packet::UINT16, ProtocolVersion);
  builder.add(Packet::UINT8, FlagNone);
  builder.add(Packet::UINT8, m_profile->genderNum());
  builder.add(Packet::UTF16, m_profile->nick());
  builder.add(Packet::UTF16, m_profile->fullName());
  builder.add(Packet::UTF16, m_profile->userAgent());
  builder.add(Packet::UTF16, m_profile->byeMsg());

  send(builder);
}


/*!
 * Слот вызывается при разрыве соединения сокетом `m_socket`.
 * Высылается сигнал `unconnected()`.
 */
void ClientService::disconnected()
{
  SCHAT_DEBUG(this << "::disconnected()" << (m_socket ? m_socket->errorString() : ""))

  if (m_ping.isActive())
    m_ping.stop();

  if (m_socket)
    m_socket->deleteLater();

  if (m_accepted) {
    emit unconnected();
    m_accepted = false;
  }

  if (!m_fatal) {
    if ((m_reconnects < (m_network->count() * 2)))
      QTimer::singleShot(0, this, SLOT(reconnect()));

    m_reconnectTimer.start();
  }
  else
    emit fatal();
}


/** [private slots]
 *
 */
void ClientService::ping()
{
  if (isReady())
    m_socket->disconnectFromHost();
}


void ClientService::reconnect()
{
  SCHAT_DEBUG(this << "::reconnect()" << m_reconnectTimer.interval() << m_reconnects << m_fatal)

  if (m_fatal)
    return;

  m_reconnects++;

  if (!m_socket)
    connectToHost();
}


/*!
 * Возвращает число активных сетевых интерфейсов, исключая LoopBack.
 */
int ClientService::activeInterfaces()
{
  int result = 0;
  QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();
  foreach (QNetworkInterface iface, allInterfaces) {
    if (iface.isValid() && iface.flags() & QNetworkInterface::IsUp && !(iface.flags() & QNetworkInterface::IsLoopBack))
      result++;
  }
  return result;
}


/*!
 * Функция создаёт сокет `m_socket` и создаёт необходимые соединения сигнал-слот.
 * ВНИМАНИЕ: функция не проверяет наличие сокета `m_socket`, это должно делаться за пределами функции.
 */
void ClientService::createSocket()
{
  SCHAT_DEBUG(this << "::createSocket()")

  m_socket = new QTcpSocket(this);
  m_stream.setDevice(m_socket);
  connect(m_socket, SIGNAL(connected()), SLOT(connected()));
  connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
  connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()));
}


void ClientService::mangleNick()
{
  SCHAT_DEBUG(this << "mangleNick()")

  int max = 99;
  QString nick = m_safeNick;
  if (nick.size() == AbstractProfile::MaxNickLength)
    nick = nick.left(AbstractProfile::MaxNickLength - 2);
  else if (nick.size() == AbstractProfile::MaxNickLength - 1)
    max = 9;

  m_profile->setNick(nick + QString().setNum(qrand() % max));
  SCHAT_DEBUG("            ^^^^^ mangled nick:" << m_profile->nick())
}


/*!
 * Разбор пакета с опкодом `OpcodeMessage`.
 */
void ClientService::messagePacket(const PacketReader &reader)
{
  QString p_sender = reader.getUtf16();
  QString p_message = reader.getUtf16();
  emit message(p_sender, p_message);
}


/*!
 * Разбор пакета с опкодом  \b OpcodeAccessDenied.
 */
void ClientService::opcodeAccessDenied(const PacketReader &reader)
{
  quint16 p_reason = reader.getUint16();

  SCHAT_DEBUG(this << "opcodeAccessDenied()" << "reason:" << p_reason)

  /// \todo Полное игнорирование ошибки \a ErrorNumericAlreadyUse не является правильным, однако эта ошибка может возникнуть при определённых обстоятельствах,
  /// что может привести к невозможности восстановления соединения, также эта ошибка возможна только при link-соединении.
  if (!(p_reason == ErrorUsersLimitExceeded || p_reason == ErrorLinksLimitExceeded || p_reason == ErrorMaxUsersPerIpExceeded || p_reason == ErrorNumericAlreadyUse))
    m_fatal = true;

  if (p_reason == ErrorNickAlreadyUse) {
    mangleNick();
    QTimer::singleShot(200, this, SLOT(connectToHost()));
    return;
  }

  emit accessDenied(p_reason);
}


/*!
 * Разбор пакета с опкодом `OpcodeAccessGranted`.
 * Функция отправляет сигнал `accessGranted(const QString &, const QString &, quint16)`.
 * Если установлено подключение к одиночному серверу, то имя сети устанавливается "".
 */
void ClientService::opcodeAccessGranted(const PacketReader &reader)
{
  quint16 p_level = reader.getUint16();
  m_accepted = true;
  m_reconnects = 0;
  m_fatal = false;

  QString network;
  if (m_network->isNetwork())
    network = m_network->name();
  else
    network = "";

  emit accessGranted(network, m_server.address, p_level);
}


/*!
 *
 */
void ClientService::opcodeLinkLeave(const PacketReader &reader)
{
  quint8 p_numeric  = reader.getUint8();
  QString p_network = reader.getUtf16();
  QString p_ip      = reader.getUtf16();

  if (p_network.isEmpty())
    return;

  if (p_ip.isEmpty())
    return;

  emit linkLeave(p_numeric, p_network, p_ip);
}


/*!
 * Разбор пакета с опкодом `OpcodeNewLink`.
 */
void ClientService::opcodeNewLink(const PacketReader &reader)
{
  quint8 p_numeric  = reader.getUint8();
  QString p_network = reader.getUtf16();
  QString p_ip      = reader.getUtf16();

  if (p_network.isEmpty())
    return;

  if (p_ip.isEmpty())
    return;

  emit newLink(p_numeric, p_network, p_ip);
}


/*!
 * Разбор пакета с опкодом `OpcodeNewNick`.
 */
void ClientService::opcodeNewNick(const PacketReader &reader)
{
  quint8 p_gender   = reader.getUint8();
  QString p_nick    = reader.getUtf16();
  QString p_newNick = reader.getUtf16();
  QString p_name    = reader.getUtf16();

  emit newNick(p_gender, p_nick, p_newNick, p_name);
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeNewProfile.
 *
 * В случае успешного разбора пакета высылается сигнал newProfile(quint8 gender, const QString &nick, const QString &name).
 */
void ClientService::opcodeNewProfile(const PacketReader &reader)
{
  SCHAT_DEBUG(this << "::opcodeNewProfile()")

  quint8 p_gender = reader.getUint8();
  QString p_nick  = reader.getUtf16();
  QString p_name  = reader.getUtf16();

  if (p_nick.isEmpty())
    return;

  emit newProfile(p_gender, p_nick, p_name);
}


/*!
 * Разбор пакета с опкодом `OpcodeNewUser`.
 * В конце разбора высылается сигнал `newUser(const QStringList &, bool)`.
 */
void ClientService::opcodeNewUser(const PacketReader &reader)
{
  quint8 p_flag    = reader.getUint8();
  quint8 p_numeric = reader.getUint8();
  quint8 p_gender  = reader.getUint8();
  QString p_nick   = reader.getUtf16();
  QString p_name   = reader.getUtf16();
  QString p_bye    = reader.getUtf16();
  QString p_agent  = reader.getUtf16();
  QString p_host   = reader.getUtf16();

  QStringList profile;
  profile << p_nick << p_name << p_bye << p_agent << p_host << AbstractProfile::gender(p_gender);

  emit newUser(profile, p_flag, p_numeric);
}


/*!
 * Разбор пакета с опкодом `OpcodePing`.
 * В ответ высылается пакет `OpcodePong`.
 */
void ClientService::opcodePing()
{
  m_ping.start();
  send(PacketBuilder(OpcodePong));
}


/*!
 * Разбор пакета с опкодом `OpcodePrivateMessage`.
 * В конце разбора высылается сигнал `privateMessage(quint8, const QString &, const QString &)`.
 */
void ClientService::opcodePrivateMessage(const PacketReader &reader)
{
  quint8 p_flag     = reader.getUint8();
  QString p_nick    = reader.getUtf16();
  QString p_message = reader.getUtf16();
  emit privateMessage(p_flag, p_nick, p_message);
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeRelayMessage.
 *
 * В случае успеха высылается сигнал void relayMessage(const QString &channel, const QString &sender, const QString &message).
 */
void ClientService::opcodeRelayMessage(const PacketReader &reader)
{
  QString p_channel = reader.getUtf16();
  QString p_sender  = reader.getUtf16();
  QString p_message = reader.getUtf16();

  SCHAT_DEBUG(this << "ClientService::opcodeRelayMessage()")
  SCHAT_DEBUG("  CHANNEL:" << p_channel)
  SCHAT_DEBUG("  SENDER: " << p_sender)
  SCHAT_DEBUG("  MESSAGE:" << p_message)
  if (p_sender.isEmpty())
    return;

  if (p_message.isEmpty())
    return;

  emit relayMessage(p_channel, p_sender, p_message);
}


/*!
 * Разбор пакета с опкодом `OpcodeServerMessage`.
 */
void ClientService::opcodeServerMessage(const PacketReader &reader)
{
  QString p_message = reader.getUtf16();
  emit serverMessage(p_message);
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeSyncByeMsg.
 */
void ClientService::opcodeSyncByeMsg(const PacketReader &reader)
{
  QString p_nick = reader.getUtf16();
  QString p_msg  = reader.getUtf16();

  if (p_nick.isEmpty())
    return;

  emit syncBye(p_nick, p_msg);
}


/*!
 * Разбор пакета с опкодом `OpcodeSyncNumerics`.
 */
void ClientService::opcodeSyncNumerics(const PacketReader &reader)
{
  QList<quint8> p_numerics = reader.getUint8List();
  emit syncNumerics(p_numerics);
}


/*!
 * Разбор универсального пакета.
 */
void ClientService::opcodeUniversal(const PacketReader &reader)
{
  quint16 subOpcode    = reader.getUint16();
  QList<quint32> data1 = reader.getUint32List();
  QStringList data2    = reader.getUtf16List();

  emit universal(subOpcode, data1, data2);
}


/*!
 * Разбор пакета с опкодом `OpcodeUserLeave`.
 * В конце разбора высылается сигнал `userLeave(const QString &, const QString &, bool)`.
 */
void ClientService::opcodeUserLeave(const PacketReader &reader)
{
  quint8 p_flag = reader.getUint8();
  QString p_nick = reader.getUtf16();
  QString p_bye  = reader.getUtf16();

  emit userLeave(p_nick, p_bye, p_flag);

  if (p_nick.toLower() == m_safeNick.toLower()) {
    m_profile->setNick(m_safeNick);
    QTimer::singleShot(0, this, SLOT(sendNewProfile()));
  }
}
