/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore>
#include <QtNetwork>

#include "clientservice.h"

static const int CheckTimeout         = 6000;
static const int ReconnectTimeout     = 3000;


/** [public]
 * 
 */
ClientService::ClientService(const AbstractProfile *profile, const Network *network, QObject *parent)
  : QObject(parent), m_profile(profile), m_network(network)
{
  m_socket = 0;
  m_nextBlockSize = 0;
  m_reconnects = 0;
  m_stream.setVersion(StreamVersion);
  m_accepted = false;
  m_fatal = false;
  m_checkTimer.setInterval(CheckTimeout);
  m_ping.setInterval(8000);
  
  connect(&m_checkTimer, SIGNAL(timeout()), SLOT(check()));
  connect(&m_reconnectTimer, SIGNAL(timeout()), SLOT(reconnect()));
  connect(&m_ping, SIGNAL(timeout()), SLOT(ping()));
}


/** [public]
 * 
 */
ClientService::~ClientService()
{
  qDebug() << "ClientService::~ClientService()";
}


/** [public]
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


/** [public]
 * 
 */
bool ClientService::sendRelayMessage(const QString &channel, const QString &sender, const QString &message, quint8 numeric)
{
#ifdef SCHAT_DEBUG
  qDebug() << "ClientService::sendRelayMessage(const QString &, const QString &, const QString &, quint8)" << channel << sender << message << numeric;
#endif
  
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0)
        << OpcodeRelayMessage
        << numeric
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


/** [public]
 * Отправка пакета `OpcodeMessage` на сервер, ник отправителя находится в удалённом сервисе.
 * const QString &channel -> канал/ник для кого предназначено сообщение (пустая строка - главный канал).
 * const QString &message -> сообщение.
 * ----
 * Возвращает `true` в случае успешной отправки (без подтверждения сервером).
 */
bool ClientService::sendMessage(const QString &channel, const QString &message)
{
#ifdef SCHAT_DEBUG
  qDebug() << "ClientService::sendMessage(const QString &, const QString &)" << channel << message;
#endif
  
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


/** [public]
 * Подключение к хосту, за выдачу адреса сервера и порта отвечает класс `m_network`.
 * В случае попытки подключения высылается сигнал `void connecting(const QString &, bool)`.
 */
void ClientService::connectToHost()
{
#ifdef SCHAT_DEBUG
  qDebug() << "ClientService::connectToHost()";
  if (m_socket)
    qDebug() << m_socket->state();
#endif
  
  if (!m_socket)
    createSocket();
  
  m_fatal = false;
  
  if (m_socket->state() == QAbstractSocket::UnconnectedState) {
    m_server = m_network->server();
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


/** [public]
 * 
 */
void ClientService::quit(bool end)
{
  qDebug() << "ClientService::quit(bool)" << end;
  
  if (m_socket) {
    qDebug() << "HAVE SOCKET";
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
    AbstractProfile profile(list);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) 
        << OpcodeNewUser
        << echo
        << numeric
        << profile.genderNum()
        << profile.nick()
        << profile.fullName()
        << profile.userAgent()
        << profile.host();

    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
  }
}


/** [private slots]
 * Разрыв соединения или переподключение если после `CheckTimeout` миллисекунд не удалось установить действующие соединение.
 */
void ClientService::check()
{
  qDebug() << "ClientService::check()";
  
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


/** [private slots]
 * Слот вызывается при успешном подключении сокета `m_socket` к серверу.
 * Слот отправляет приветственное сообщение серверу (OpcodeGreeting).
 * Таймер переподключения `m_reconnectTimer` отстанавливается.
 */
void ClientService::connected()
{
#ifdef SCHAT_DEBUG
  qDebug() << "ClientService::connected()";
#endif
  
  m_nextBlockSize = 0;
  m_reconnectTimer.stop();
  m_reconnectTimer.setInterval(ReconnectTimeout);
  
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


/** [private slots]
 * Слот вызывается при разрыве соединения сокетом `m_socket`.
 * Высылается сигнал `unconnected()`.
 */
void ClientService::disconnected()
{
#ifdef SCHAT_DEBUG
  qDebug() << "ClientService::disconnected()";
#endif

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


/** [private slots]
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
    
    if (m_opcode != 400)
      qDebug() << "op" << m_opcode;
    
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
          m_nextBlockSize = 0;
          emit syncUsersEnd();
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


/** [private slots]
 * 
 */
void ClientService::reconnect()
{
#ifdef SCHAT_DEBUG
  qDebug() << "ClientService::reconnect()" << m_reconnectTimer.interval() << m_reconnects << m_fatal;
#endif

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


/** [private]
 * Функция создаёт сокет `m_socket` и создаёт необходимые соединения сигнал-слот.
 * ВНИМАНИЕ: функция не проверяет наличие сокета `m_socket`, это должно делаться за пределами функции.
 */
void ClientService::createSocket()
{
#ifdef SCHAT_DEBUG
  qDebug() << "ClientService::createSocket()";
#endif
  
  m_socket = new QTcpSocket(this);
  m_stream.setDevice(m_socket);
  connect(m_socket, SIGNAL(connected()), SLOT(connected()));
  connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
  connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()));
}


/** [private]
 * Разбор пакета с опкодом `OpcodeAccessDenied`.
 */
void ClientService::opcodeAccessDenied()
{
  quint16 p_reason;
  m_stream >> p_reason;
  m_nextBlockSize = 0;
  qDebug() << "reason" << p_reason;
  m_fatal = true;
  emit accessDenied(p_reason);
}


/** [private]
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


/** [private]
 * 
 */
void ClientService::opcodeNewProfile()
{
  qDebug() << "ClientService::opcodeNewProfile()";
  
  quint8 p_gender;
  QString p_nick;
  QString p_name;
  m_stream >> p_gender >> p_nick >> p_name;
  m_nextBlockSize = 0;
  
  bool echo = true;
  if (m_profile->nick() == p_nick)
    echo = false;
  
  emit newProfile(p_gender, p_nick, p_name, echo);
}


/** [private]
 * Разбор пакета с опкодом `OpcodeNewUser`.
 * В конце разбора высылается сигнал `newUser(const QStringList &, bool)`.
 */
void ClientService::opcodeNewUser()
{
  qDebug() << "ClientService::opcodeNewUser()";

  quint8 p_flag;
  quint8 p_numeric;
  quint8 p_gender;
  QString p_nick;
  QString p_name;
  QString p_agent;
  QString p_host;
  
  m_stream >> p_flag >> p_numeric >> p_gender >> p_nick >> p_name >> p_agent >> p_host;
  m_nextBlockSize = 0;
  QStringList profile;
  profile << p_nick << p_name << "" << p_agent << p_host << AbstractProfile::gender(p_gender);

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


/** [private]
 * Разбор пакета с опкодом `OpcodeRelayMessage`.
 */
void ClientService::opcodeRelayMessage()
{
  quint8 p_numeric;
  QString p_channel;
  QString p_sender;
  QString p_message;
  m_stream >> p_numeric >> p_channel >> p_sender >> p_message;
  m_nextBlockSize = 0;
#ifdef SCHAT_DEBUG
  qDebug() << "ClientService::opcodeRelayMessage()";
  qDebug() << "  CHANNEL:" << p_channel;
  qDebug() << "  SENDER: " << p_sender;
  qDebug() << "  MESSAGE:" << p_message;
  qDebug() << "  NUMERIC:" << p_numeric;
#endif
  emit relayMessage(p_channel, p_sender, p_message, p_numeric);
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


/** [private]
 * Разбор пакета с опкодом `OpcodeSyncNumerics`.
 */
void ClientService::opcodeSyncNumerics()
{
  QList<quint8> p_numerics;
  m_stream >> p_numerics;
  m_nextBlockSize = 0;
  emit syncNumerics(p_numerics);
}


/** [private]
 * Разбор пакета с опкодом `OpcodeUserLeave`.
 * В конце разбора высылается сигнал `userLeave(const QString &, const QString &, bool)`.
 */
void ClientService::opcodeUserLeave()
{
  bool echo;
  quint8 p_flag;
  QString p_nick;
  QString p_bye;
  m_stream >> p_flag >> p_nick >> p_bye;
  m_nextBlockSize = 0;
  
  if (p_flag)
    echo = true;
  else
    echo = false;
  
  emit userLeave(p_nick, p_bye, echo);
}


/** [private]
 * Функция читает пакет с неизвестным опкодом.
 */
void ClientService::unknownOpcode()
{
  qDebug() << "DaemonService::unknownOpcode()";
  qDebug() << "opcode:" << m_opcode << "size:" << m_nextBlockSize;
  QByteArray block = m_socket->read(m_nextBlockSize - (int) sizeof(quint16));
  m_nextBlockSize = 0;
}
