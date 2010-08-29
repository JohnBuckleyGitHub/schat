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

#include "abstractprofile.h"
#include "channellog.h"
#include "daemonservice.h"
#include "packet.h"
#include "schat.h"

/*!
 * \brief Конструктор класса DaemonService.
 */
DaemonService::DaemonService(QTcpSocket *socket, QObject *parent)
  : AbstractPeer(parent)
{
  SCHAT_DEBUG(this);
  m_socket = socket;

  if (m_socket) {
    m_socket->setParent(this);
    connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()));
    m_accepted = false;
    m_nextBlockSize = 0;
    m_stream.setDevice(m_socket);
    m_stream.setVersion(StreamVersion);
    m_pings = 0;
    m_ping.start(9000);
    m_numeric = 0;
    m_kill = false;
    connect(&m_ping, SIGNAL(timeout()), SLOT(ping()));
  }
  else
    deleteLater();
}


QString DaemonService::host() const
{
  return m_socket->peerAddress().toString();
}


/*!
 * Клиент получил отказ в доступе, `quint16 reason` - причина отказа.
 * Отсылаем ошибку и разрываем соединение.
 * \deprecated Необходимо заменить пакет OpcodeAccessDenied на HandshakeReply.
 */
void DaemonService::accessDenied(quint16 reason)
{
  PacketBuilder builder(OpcodeAccessDenied);
  builder.add(Packet::UINT16, reason);
  send(builder);

  m_socket->disconnectFromHost();
}


/*!
 * Клиент успешно получил доступ, отсылаем уведомление об успешном доступе
 * и устанавливаем флаг `m_accepted` в `true`.
 * \deprecated Необходимо заменить пакет OpcodeAccessGranted на HandshakeReply.
 */
void DaemonService::accessGranted(quint16 numeric)
{
  if (!m_accepted) {
    PacketBuilder builder(OpcodeAccessGranted);
    builder.add(Packet::UINT16, numeric);
    send(builder);
    m_accepted = true;
  }
}


/*!
 *
 */
void DaemonService::quit(bool kill)
{
  m_kill = kill;

  if (isReady())
    m_socket->disconnectFromHost();
}


/*!
 * Пакет `OpcodeSyncNumerics`.
 */
void DaemonService::sendNumerics(const QList<quint8> &numerics)
{
  PacketBuilder builder(OpcodeSyncNumerics);
  builder.add(numerics);
  send(builder);
}


void DaemonService::sendServerMessage(const QString &msg)
{
  PacketBuilder builder(OpcodeServerMessage);
  builder.add(Packet::UTF16, msg);

  send(builder);
}


void DaemonService::sendSyncUsersEnd()
{
  send(PacketBuilder(OpcodeSyncUsersEnd));
}


/*!
 * Отправка универсального пакета.
 *
 * \param sub   Субопкод.
 * \param data1 Список данных типа quint32
 * \param data2 Список данных типа QString
 */
bool DaemonService::sendUniversal(quint16 sub, const QList<quint32> &data1, const QStringList &data2)
{
  PacketBuilder builder(OpcodeUniversal);
  builder.add(Packet::UINT16, sub);
  builder.add(data1);
  builder.add(data2);

  return send(builder);}



void DaemonService::sendMessage(const QString &sender, const QString &message)
{
  PacketBuilder builder(Packet::Message);
  builder.add(Packet::UTF16, sender);
  builder.add(Packet::UTF16, message);

  send(builder);
}


/*!
 * \brief Отправка пакета с опкодом \b OpcodeNewNick.
 *
 */
void DaemonService::sendNewNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name)
{
  SCHAT_DEBUG(this << "::sendNewNick()")

  if (m_profile->nick() == nick && m_flag == FlagNone) {
    m_profile->setGender(gender);
    m_profile->setNick(newNick);
    m_profile->setFullName(name);
  }

  PacketBuilder builder(OpcodeNewNick);
  builder.add(Packet::UINT8, gender);
  builder.add(Packet::UTF16, nick);
  builder.add(Packet::UTF16, newNick);
  builder.add(Packet::UTF16, name);

  send(builder);
}


/*!
 * \brief Отправка пакета с опкодом \b OpcodeNewProfile.
 *
 */
void DaemonService::sendNewProfile(quint8 gender, const QString &nick, const QString &name)
{
  SCHAT_DEBUG(this << "::sendNewProfile()")

  if (m_profile->nick() == nick) {
    m_profile->setGender(gender);
    m_profile->setFullName(name);
  }

  sendPacket(OpcodeNewProfile, gender, nick, name);
}


/*!
 * Формирует и отправляет пакет с опкодом `OpcodeNewUser`.
 */
void DaemonService::sendNewUser(const QStringList &list, quint8 echo, quint8 numeric)
{
  if (isReady()) {

    if (m_flag == FlagNone)
      if (list.at(AbstractProfile::Nick) == m_profile->nick() && !echo)
        return;

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
}


void DaemonService::sendRelayMessage(const QString &channel, const QString &sender, const QString &message)
{
  PacketBuilder builder(OpcodeRelayMessage);
  builder.add(Packet::UTF16, channel);
  builder.add(Packet::UTF16, sender);
  builder.add(Packet::UTF16, message);

  send(builder);
}


void DaemonService::sendSyncBye(const QString &nick, const QString &bye)
{
  PacketBuilder builder(OpcodeSyncByeMsg);
  builder.add(Packet::UTF16, nick);
  builder.add(Packet::UTF16, bye);

  send(builder);
}


/*!
 * \todo Добавить поддержку склеенных пакетов.
 */
void DaemonService::readPacket(int pcode, const QByteArray &block)
{
  AbstractPeer::readPacket(pcode, block);

  PacketReader reader(pcode, block);

  if (m_accepted) {
    switch (pcode) {
      case Packet::Message:
        messagePacket(reader);
        break;

      case OpcodePong:
        m_pings = 0;
        break;

      case OpcodeNewProfile:
        opcodeNewProfile(reader);
        break;

      case OpcodeByeMsg:
        opcodeByeMsg(reader);
        break;

      case OpcodeRelayMessage:
        opcodeRelayMessage(reader);
        break;

      case OpcodeNewUser:
        opcodeNewUser(reader);
        break;

      case OpcodeUserLeave:
        opcodeUserLeave(reader);
        break;

      case OpcodeNewNick:
        opcodeNewNick(reader);
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
  else if (pcode == OpcodeGreeting) {
    if (!opcodeGreeting(reader)) {
      m_socket->disconnectFromHost();
      return;
    }
  }
  else {
    m_socket->disconnectFromHost();
    return;
  }


}


/*!
 * Обработка разрыва соединения.
 */
void DaemonService::disconnected()
{
  if (m_accepted && !m_kill) {
    if (m_error.isEmpty())
      m_error = m_socket->errorString();

    emit leave(m_profile->nick(), m_flag, m_error);
  }

  deleteLater();
}


/*!
 * Слот вызывается таймером \a m_ping, для проверки состояния соединения.
 *
 * Если соединение успешно установлено (m_accepted == true) и счётчик отправленных пакетов \a m_pings
 * не превышен, производится отправка пакета \b OpcodePing и увеличивается счётчик пингов,
 * иначе \a m_error устанавливается в "Ping timeout" и происходит отключение.
 *
 * При (m_accepted == false) при активном соединения происходит разъединение, иначе удаление сокета.
 */
void DaemonService::ping()
{
  if (m_accepted) {
    if (m_pings < 2) {
      send(PacketBuilder(OpcodePing));
      ++m_pings;
    }
    else {
      m_error = tr("Ping timeout");
      m_socket->disconnectFromHost();
    }
  }
  else {
    if (m_socket->state() == QAbstractSocket::ConnectedState)
      m_socket->disconnectFromHost();
    else
      deleteLater();
  }
}


bool DaemonService::opcodeGreeting(const PacketReader &reader)
{
  SCHAT_DEBUG(this << "::opcodeGreeting()")

  quint16 p_version   = reader.getUint16();
  m_flag              = reader.getUint8();
  quint8  p_gender    = reader.getUint8();
  QString p_nick      = reader.getUtf16();
  QString p_name      = reader.getUtf16();
  QString p_userAgent = reader.getUtf16();
  QString p_byeMsg    = reader.getUtf16();

  QStringList profile;
  profile << p_nick << p_name << p_byeMsg << p_userAgent << m_socket->peerAddress().toString() << AbstractProfile::gender(p_gender);
  qDebug() << profile;
  m_profile = new AbstractProfile(profile, this);

  quint16 err = verifyGreeting(p_version);

  if (err) {
    accessDenied(err);
    return false;
  }

  emit greeting(m_profile->pack(), m_flag);

  return true;
}


/*!
 * Отправка стандартного пакета:
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  ->
 * QString ->
 * QString ->
 * ОПКОДЫ:
 *   `OpcodePrivateMessage`, `OpcodeUserLeave`.
 */
bool DaemonService::sendPacket(int pcode, int flag, const QString &nick, const QString &message)
{
  PacketBuilder builder(pcode);
  builder.add(Packet::UINT8, flag);
  builder.add(Packet::UTF16, nick);
  builder.add(Packet::UTF16, message);

  return send(builder);
}

/*!
 * Обнаружение команды "/all".
 */
QString DaemonService::parseCmd(const QString &message) const
{
  QString text = ChannelLog::toPlainText(message).trimmed().toLower();
  QString out = message;
  if (text.startsWith("/all ", Qt::CaseInsensitive)) {
    QString cmd = "/all ";
    int index = out.indexOf(cmd, 0, Qt::CaseInsensitive);
    if (index == -1) {
      cmd = "/all";
      index = out.indexOf(cmd, 0, Qt::CaseInsensitive);
    }
    if (index != -1)
      out.remove(index, cmd.size());

    out = "<b>" + m_profile->nick() + "</b> " + out;
  }

  return out;
}


/** [private]
 * Верификация пакета `OpcodeGreeting`.
 */
quint16 DaemonService::verifyGreeting(quint16 version)
{
  if (version < ProtocolVersion)
    return ErrorOldClientProtocol;

  if (version > ProtocolVersion)
    return ErrorOldServerProtocol;

  if (!(m_flag == FlagNone || m_flag == FlagLink))
    return ErrorBadGreetingFlag;

  if (!m_profile->isValidNick() && m_flag == FlagNone)
    return ErrorBadNickName;

  if (!m_profile->isValidUserAgent())
    return ErrorBadUserAgent;

  if (m_flag == FlagLink) {
    bool ok;
    m_numeric = quint8(m_profile->nick().toInt(&ok));
    if (ok) {
      if (!m_numeric)
        return ErrorBadNumeric;
    }
    else
      return ErrorBadNumeric;
  }

  return 0;
}


/*!
 * \brief Разбор пакета \b OpcodeMessage, полученного от клиента.
 *
 * В случае успеха высылается сигнал message(const QString &channel, const QString &sender, const QString &message).
 */
void DaemonService::messagePacket(const PacketReader &reader)
{
  QString p_channel = reader.getUtf16();
  QString p_message = reader.getUtf16();

  SCHAT_DEBUG(this << "Packet::Message")
  SCHAT_DEBUG("  CHANNEL:" << p_channel)
  SCHAT_DEBUG("  SENDER: " << m_profile->nick())
  SCHAT_DEBUG("  MESSAGE:" << p_message)

  if (p_message.isEmpty())
    return;

  p_message = ChannelLog::htmlFilter(p_message);
  if (p_message.isEmpty())
    return;

  emit message(p_channel, m_profile->nick(), p_message);
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeByeMsg.
 */
void DaemonService::opcodeByeMsg(const PacketReader &reader)
{
  QString p_bye = reader.getUtf16();
  m_profile->setByeMsg(p_bye);
  emit newBye(m_profile->nick(), p_bye);
}


void DaemonService::opcodeNewNick(const PacketReader &reader)
{
  SCHAT_DEBUG(this << "::opcodeNewNick()")

  quint8 p_gender   = reader.getUint8();
  QString p_nick    = reader.getUtf16();
  QString p_newNick = reader.getUtf16();
  QString p_name    = reader.getUtf16();

  emit newNick(p_gender, p_nick, p_newNick, p_name);
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeNewProfile.
 *
 *
 */
void DaemonService::opcodeNewProfile(const PacketReader &reader)
{
  SCHAT_DEBUG(this << "::opcodeNewProfile()")

  quint8 p_gender = reader.getUint8();
  QString p_nick  = reader.getUtf16();
  QString p_name  = reader.getUtf16();

  if (p_nick.isEmpty())
    return;

  if (m_profile->nick() == p_nick)
    emit newProfile(p_gender, m_profile->nick(), p_name);
  else
    emit newNick(p_gender, m_profile->nick(), p_nick, p_name);
}


void DaemonService::opcodeNewUser(const PacketReader &reader)
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
 * \brief Разбор пакета с опкодом \b OpcodeRelayMessage.
 *
 * В случае успеха высылается сигнал void relayMessage(const QString &channel, const QString &sender, const QString &message).
 */
void DaemonService::opcodeRelayMessage(const PacketReader &reader)
{
  QString p_channel = reader.getUtf16();
  QString p_sender  = reader.getUtf16();
  QString p_message = reader.getUtf16();

  SCHAT_DEBUG(this << "::opcodeRelayMessage()")
  SCHAT_DEBUG(this << "  CHANNEL:" << p_channel)
  SCHAT_DEBUG(this << "  SENDER: " << p_sender)
  SCHAT_DEBUG(this << "  MESSAGE:" << p_message)

  if (p_sender.isEmpty())
    return;

  if (p_message.isEmpty())
    return;

  p_message = ChannelLog::htmlFilter(p_message);
  if (p_message.isEmpty())
    return;

  emit relayMessage(p_channel, p_sender, p_message);
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeSyncByeMsg.
 */
void DaemonService::opcodeSyncByeMsg(const PacketReader &reader)
{
  QString p_nick = reader.getUtf16();
  QString p_msg  = reader.getUtf16();

  if (p_nick.isEmpty())
    return;

  emit newBye(p_nick, p_msg);
}


/*!
 * Разбор универсального пакета.
 */
void DaemonService::opcodeUniversal(const PacketReader &reader)
{
  quint16        subOpcode = reader.getUint16();
  QList<quint32> data1     = reader.getUint32List();
  QStringList    data2     = reader.getUtf16List();

  if (m_flag == FlagLink)
    emit universal(subOpcode, data1, data2, m_numeric);
  else
    emit universal(subOpcode, m_profile->nick(), data1, data2);
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeUserLeave.
 *
 * В конце разбора высылается сигнал userLeave(const QString &, const QString &, bool).
 */
void DaemonService::opcodeUserLeave(const PacketReader &reader)
{
  quint8 p_flag  = reader.getUint8();
  QString p_nick = reader.getUtf16();
  QString p_bye  = reader.getUtf16();

  emit userLeave(p_nick, p_bye, p_flag);
}
