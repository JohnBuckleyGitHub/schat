/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include <QNetworkInterface>

#include "clientservice.h"

static const int CheckTimeout         = 15000;
static const int ReconnectTimeout     = 4000;

#ifdef SCHAT_DEBUG
  #undef SCHAT_DEBUG
  #define SCHAT_DEBUG(x) qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << x;
  #include <QDebug>
  #include <QTime>
#else
  #define SCHAT_DEBUG(x)
#endif

/*!
 * \brief Конструктор класса ClientService.
 */
ClientService::ClientService(AbstractProfile *profile, const Network *network, QObject *parent)
  : QObject(parent),
  m_profile(profile),
  m_accepted(false),
  m_fatal(false),
  m_synced(false),
  m_network(network),
  m_reconnects(0),
  m_socket(0),
  m_safeNick(profile->nick()),
  m_nextBlockSize(0)
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
 * Возвращает `true` если сервис находится в активном состоянии.
 */
bool ClientService::isReady() const
{
  if (m_socket) {
    if (m_socket->state() == QTcpSocket::ConnectedState && m_accepted)
      return true;
    else
      return false;
  }
  else
    return false;
}


bool ClientService::sendRelayMessage(const QString &channel, const QString &sender, const QString &message)
{
  SCHAT_DEBUG(this << "::sendRelayMessage(const QString &, const QString &, const QString &, quint8)" << channel << sender << message)

  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0)
        << OpcodeRelayMessage
        << channel
        << sender
        << message;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
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
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << OpcodeUniversal << sub << data1 << data2;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
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

  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0)
        << OpcodeMessage
        << channel
        << message;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
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


/** [public]
 *
 */
void ClientService::sendNewUser(const QStringList &list, quint8 echo, quint8 numeric)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0)
        << OpcodeNewUser
        << echo
        << numeric
        << AbstractProfile::genderNum(list.at(AbstractProfile::Gender))
        << list.at(AbstractProfile::Nick)
        << list.at(AbstractProfile::FullName)
        << list.at(AbstractProfile::ByeMsg)
        << list.at(AbstractProfile::UserAgent)
        << list.at(AbstractProfile::Host);

    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
  }
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
  m_synced = false;

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


/*!
 * Разрыв соединения или переподключение если после `CheckTimeout` миллисекунд не удалось установить действующие соединение.
 */
void ClientService::check()
{
  SCHAT_DEBUG(this << "::check()" << m_accepted << m_synced)

  if (m_socket) {
    if (m_socket->state() == QTcpSocket::ConnectedState) {
      if (!m_accepted || !m_synced) {
        m_socket->disconnectFromHost();
        return;
      }
      m_checkTimer.stop();
    }
    else {
      m_socket->deleteLater();
      m_socket = 0;
      QTimer::singleShot(1000, this, SLOT(connectToHost()));
      return;
    }
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

  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(StreamVersion);
  out << quint16(0)
      << OpcodeGreeting
      << ProtocolVersion
      #ifdef SCHAT_CLIENT
       << FlagNone
      #else
       << FlagLink
      #endif
      << m_profile->genderNum()
      << m_profile->nick()
      << m_profile->fullName()
      << m_profile->userAgent()
      << m_profile->byeMsg();

  out.device()->seek(0);
  out << quint16(block.size() - (int) sizeof(quint16));
  m_socket->write(block);
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


/*!
 * Слот вызывается когда поступила новая порция данных для чтения из сокета `m_socket`.
 */
void ClientService::readyRead()
{
  forever {
    if (!m_nextBlockSize) {
      if (m_socket->bytesAvailable() < (int) sizeof(quint16))
        break;

      m_stream >> m_nextBlockSize;
    }

    if (m_socket->bytesAvailable() < m_nextBlockSize)
      break;

    m_stream >> m_opcode;

    #ifdef SCHAT_DEBUG
    if (m_opcode != 400) {
      SCHAT_DEBUG(this << "client opcode:" << m_opcode << "size:" << m_nextBlockSize)
    }
    #endif

    if (m_accepted) {
      switch (m_opcode) {
        case OpcodeNewUser:
          opcodeNewUser();
          break;

        case OpcodeUserLeave:
          opcodeUserLeave();
          break;

        case OpcodeMessage:
          opcodeMessage();
          break;

        case OpcodePrivateMessage:
          opcodePrivateMessage();
          break;

        case OpcodePing:
          opcodePing();
          break;

        case OpcodeNewProfile:
          opcodeNewProfile();
          break;

        case OpcodeNewNick:
          opcodeNewNick();
          break;

        case OpcodeServerMessage:
          opcodeServerMessage();
          break;

        case OpcodeNewLink:
          opcodeNewLink();
          break;

        case OpcodeLinkLeave:
          opcodeLinkLeave();
          break;

        case OpcodeRelayMessage:
          opcodeRelayMessage();
          break;

        case OpcodeSyncNumerics:
          opcodeSyncNumerics();
          break;

        case OpcodeSyncUsersEnd:
          opcodeSyncUsersEnd();
          break;

        case OpcodeSyncByeMsg:
          opcodeSyncByeMsg();
          break;

        case OpcodeUniversal:
          opcodeUniversal();
          break;

        default:
          unknownOpcode();
          break;
      };
    }
    else if (m_opcode == OpcodeAccessGranted) {
      opcodeAccessGranted();
    }
    else if (m_opcode == OpcodeAccessDenied) {
      opcodeAccessDenied();
    }
    else {
      m_socket->disconnectFromHost();
      return;
    }
  }
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


/** [private]
 * Отправка стандартного пакета:
 * quint16 -> размер пакета
 * quint16 -> опкод
 * ОПКОДЫ:
 *   `OpcodePong`.
 */
bool ClientService::send(quint16 opcode)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
}


/** [private]
 * Отправка стандартного пакета:
 * quint16 -> размер пакета
 * quint16 -> опкод
 * QString ->
 * ОПКОДЫ:
 *   `OpcodeByeMsg`.
 */
bool ClientService::send(quint16 opcode, const QString &msg)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode << msg;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
}


bool ClientService::send(quint16 opcode, const QString &str1, const QString &str2)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode << str1 << str2;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
}


/** [private]
 * Отправка стандартного пакета:
 * quint16 -> размер пакета
 * quint16 -> опкод
 * quint8  ->
 * QString ->
 * QString ->
 * ОПКОДЫ:
 *   `OpcodeNewProfile`.
 */
bool ClientService::send(quint16 opcode, quint8 gender, const QString &nick, const QString &name)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode << gender << nick << name;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
}


bool ClientService::send(quint16 opcode, quint8 gender, const QString &nick, const QString &nNick, const QString &name)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode << gender << nick << nNick << name;
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
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
 * Разбор пакета с опкодом  \b OpcodeAccessDenied.
 */
void ClientService::opcodeAccessDenied()
{
  quint16 p_reason;
  m_stream >> p_reason;
  m_nextBlockSize = 0;

  SCHAT_DEBUG(this << "opcodeAccessDenied()" << "reason:" << p_reason)

  /// \todo Полное игнорирование ошибки \a ErrorNumericAlreadyUse не является правильным, однако эта ошибка может возникнуть при определённых обстоятельствах,
  /// что может привести к невозможности восстановления соединения, также эта ошибка возможна только при link-соединении.
  if (!(p_reason == ErrorUsersLimitExceeded || p_reason == ErrorLinksLimitExceeded || p_reason == ErrorMaxUsersPerIpExceeded || p_reason == ErrorNumericAlreadyUse))
    m_fatal = true;

  if (p_reason == ErrorNickAlreadyUse) {
    mangleNick();
    QTimer::singleShot(400, this, SLOT(connectToHost()));
    return;
  }

  emit accessDenied(p_reason);
}


/*!
 * Разбор пакета с опкодом `OpcodeAccessGranted`.
 * Функция отправляет сигнал `accessGranted(const QString &, const QString &, quint16)`.
 * Если установлено подключение к одиночному серверу, то имя сети устанавливается "".
 */
void ClientService::opcodeAccessGranted()
{
  quint16 p_level;
  m_stream >> p_level;
  m_nextBlockSize = 0;
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


/** [private]
 *
 */
void ClientService::opcodeLinkLeave()
{
  quint8 p_numeric;
  QString p_network;
  QString p_ip;
  m_stream >> p_numeric >> p_network >> p_ip;
  m_nextBlockSize = 0;

  if (p_network.isEmpty())
    return;

  if (p_ip.isEmpty())
    return;

  emit linkLeave(p_numeric, p_network, p_ip);
}


/** [private]
 * Разбор пакета с опкодом `OpcodeMessage`.
 * В конце разбора высылается сигнал `message(const QString &, const QString &)`.
 */
void ClientService::opcodeMessage()
{
  QString p_sender;
  QString p_message;
  m_stream >> p_sender >> p_message;
  m_nextBlockSize = 0;
  emit message(p_sender, p_message);
}


/** [private]
 * Разбор пакета с опкодом `OpcodeNewLink`.
 */
void ClientService::opcodeNewLink()
{
  quint8 p_numeric;
  QString p_network;
  QString p_ip;
  m_stream >> p_numeric >> p_network >> p_ip;
  m_nextBlockSize = 0;

  if (p_network.isEmpty())
    return;

  if (p_ip.isEmpty())
    return;

  emit newLink(p_numeric, p_network, p_ip);
}


/** [private]
 * Разбор пакета с опкодом `OpcodeNewNick`.
 */
void ClientService::opcodeNewNick()
{
  quint8 p_gender;
  QString p_nick;
  QString p_newNick;
  QString p_name;
  m_stream >> p_gender >> p_nick >> p_newNick >> p_name;
  m_nextBlockSize = 0;
  emit newNick(p_gender, p_nick, p_newNick, p_name);
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeNewProfile.
 *
 * В случае успешного разбора пакета высылается сигнал newProfile(quint8 gender, const QString &nick, const QString &name).
 */
void ClientService::opcodeNewProfile()
{
  SCHAT_DEBUG(this << "::opcodeNewProfile()")

  quint8 p_gender;
  QString p_nick;
  QString p_name;
  m_stream >> p_gender >> p_nick >> p_name;
  m_nextBlockSize = 0;

  if (p_nick.isEmpty())
    return;

  emit newProfile(p_gender, p_nick, p_name);
}


/** [private]
 * Разбор пакета с опкодом `OpcodeNewUser`.
 * В конце разбора высылается сигнал `newUser(const QStringList &, bool)`.
 */
void ClientService::opcodeNewUser()
{
  quint8 p_flag;
  quint8 p_numeric;
  quint8 p_gender;
  QString p_nick;
  QString p_name;
  QString p_bye;
  QString p_agent;
  QString p_host;

  m_stream >> p_flag >> p_numeric >> p_gender >> p_nick >> p_name >> p_bye >> p_agent >> p_host;
  m_nextBlockSize = 0;
  QStringList profile;
  profile << p_nick << p_name << p_bye << p_agent << p_host << AbstractProfile::gender(p_gender);

  emit newUser(profile, p_flag, p_numeric);
}


/** [private]
 * Разбор пакета с опкодом `OpcodePing`.
 * В ответ высылается пакет `OpcodePong`.
 */
void ClientService::opcodePing()
{
  m_nextBlockSize = 0;
  m_ping.start();
  send(OpcodePong);
}


/** [private]
 * Разбор пакета с опкодом `OpcodePrivateMessage`.
 * В конце разбора высылается сигнал `privateMessage(quint8, const QString &, const QString &)`.
 */
void ClientService::opcodePrivateMessage()
{
  quint8 p_flag;
  QString p_nick;
  QString p_message;
  m_stream >> p_flag >> p_nick >> p_message;
  m_nextBlockSize = 0;
  emit privateMessage(p_flag, p_nick, p_message);
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeRelayMessage.
 *
 * В случае успеха высылается сигнал void relayMessage(const QString &channel, const QString &sender, const QString &message).
 */
void ClientService::opcodeRelayMessage()
{
  QString p_channel;
  QString p_sender;
  QString p_message;
  m_stream >> p_channel >> p_sender >> p_message;
  m_nextBlockSize = 0;
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


/** [private]
 * Разбор пакета с опкодом `OpcodeServerMessage`.
 */
void ClientService::opcodeServerMessage()
{
  QString p_message;
  m_stream >> p_message;
  m_nextBlockSize = 0;
  emit serverMessage(p_message);
}


/*!
 * \brief Разбор пакета с опкодом \b OpcodeSyncByeMsg.
 */
void ClientService::opcodeSyncByeMsg()
{
  QString p_nick;
  QString p_msg;
  m_stream >> p_nick >> p_msg;
  m_nextBlockSize = 0;

  if (p_nick.isEmpty())
    return;

  emit syncBye(p_nick, p_msg);
}


/*!
 * Разбор пакета с опкодом `OpcodeSyncNumerics`.
 */
void ClientService::opcodeSyncNumerics()
{
  QList<quint8> p_numerics;
  m_stream >> p_numerics;
  m_nextBlockSize = 0;
  emit syncNumerics(p_numerics);
}


void ClientService::opcodeSyncUsersEnd()
{
  m_nextBlockSize = 0;
  m_synced = true;
  emit syncUsersEnd();
}


/*!
 * Разбор универсального пакета.
 */
void ClientService::opcodeUniversal()
{
  quint16        subOpcode;
  QList<quint32> data1;
  QStringList    data2;
  m_stream >> subOpcode >> data1 >> data2;
  m_nextBlockSize = 0;

  emit universal(subOpcode, data1, data2);
}


/*!
 * Разбор пакета с опкодом `OpcodeUserLeave`.
 * В конце разбора высылается сигнал `userLeave(const QString &, const QString &, bool)`.
 */
void ClientService::opcodeUserLeave()
{
  quint8 p_flag;
  QString p_nick;
  QString p_bye;
  m_stream >> p_flag >> p_nick >> p_bye;
  m_nextBlockSize = 0;

  emit userLeave(p_nick, p_bye, p_flag);

  if (p_nick.toLower() == m_safeNick.toLower()) {
    m_profile->setNick(m_safeNick);
    QTimer::singleShot(0, this, SLOT(sendNewProfile()));
  }
}


/*!
 * Функция читает пакет с неизвестным опкодом.
 */
void ClientService::unknownOpcode()
{
  SCHAT_DEBUG(this << "::unknownOpcode()")
  SCHAT_DEBUG("opcode:" << m_opcode << "size:" << m_nextBlockSize)

  QByteArray block = m_socket->read(m_nextBlockSize - (int) sizeof(quint16));
  m_nextBlockSize = 0;
}
