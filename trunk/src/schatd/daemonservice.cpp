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

#include "daemonservice.h"
#include "abstractprofile.h"
#include "protocol.h"


/** [public]
 * 
 */
DaemonService::DaemonService(QTcpSocket *socket, QObject *parent)
: QObject(parent), m_socket(socket)
{
  qDebug() << "DaemonService::KONSTRUCTOR";
  
  if (m_socket) {
    connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()));
    m_accepted = false;
    m_nextBlockSize = 0;
    m_stream.setDevice(m_socket);
    m_stream.setVersion(StreamVersion);
    m_pings = 0;
    m_ping.start(6000);
    m_numeric = 0;
    connect(&m_ping, SIGNAL(timeout()), SLOT(ping()));
  }
  else
    deleteLater();
}


/** [public]
 * 
 */
DaemonService::~DaemonService()
{
  qDebug() << "DaemonService::~DESTRUCTOR";
  
  if (m_socket)
    delete m_socket;
}


/** [public]
 * Возвращает `true` если сервис находится в активном состоянии.
 */
bool DaemonService::isReady() const
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
 * Клиент получил отказ в доступе, `quint16 reason` - причина отказа.
 * Отсылаем ошибку и разрываем соединение.
 */
void DaemonService::accessDenied(quint16 reason)
{
  send(OpcodeAccessDenied, reason);
  m_socket->disconnectFromHost();
}


/** [public]
 * Клиент успешно получил доступ, отсылаем уведомление об успешном доступе
 * и устанавливаем флаг `m_accepted` в `true`.
 */
void DaemonService::accessGranted(quint16 level)
{
  if (!m_accepted) {
    send(OpcodeAccessGranted, level);
    m_accepted = true;
  }
}


/** [public]
 * Отправка пакета с опкодом `OpcodePrivateMessage`.
 */
void DaemonService::privateMessage(quint8 flag, const QString &nick, const QString &message)
{
  send(OpcodePrivateMessage, flag, nick, message);
}


/** [public]
 * 
 */
void DaemonService::quit()
{
  if (isReady())
    m_socket->disconnectFromHost();
}


/** [public]
 * Пакет `OpcodeSyncNumerics`.
 */
void DaemonService::sendNumerics(const QList<quint8> &numerics)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << OpcodeSyncNumerics << numerics; 
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
  }
}


/** [public]
 * 
 */
void DaemonService::sendServerMessage(const QString &msg)
{
  send(OpcodeServerMessage, msg);
}


/** [public slots]
 * Формирует и отправляет пакет с опкодом `OpcodeNewUser`.
 */
bool DaemonService::newUser(const QStringList &list, quint8 echo, quint8 numeric)
{
  qDebug() << "DaemonService::newUser(const QStringList &)" << list.at(AbstractProfile::Nick);
  
  if (isReady()) {
    AbstractProfile profile(list);
    
    if (profile.nick() == m_profile->nick() && !echo)
      return true;
    
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
    return true;
  }
  else
    return false;  
}


/** [public slots]
 * Формирует и отправляет пакет с опкодом `OpcodeUserLeave`.
 */
bool DaemonService::userLeave(const QString &nick, const QString &bye, bool echo)
{
  if (m_socket->state() == QTcpSocket::ConnectedState) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << OpcodeUserLeave;
    
    if (echo)
      out << quint8(1);
    else
      out << quint8(0);
    
    out << nick
        << bye;
    
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);    
    return true;
  }
  else
    return false;  
}


/** [public slots]
 * 
 */
void DaemonService::disconnected()
{
  qDebug() << "DaemonService::disconnected()";
  
  if (m_accepted)
    emit leave(m_profile->nick(), m_flag);
  
  deleteLater();
}


/** [public slots]
 * Отправка пакета с опкодом `OpcodeMessage`.
 */
void DaemonService::message(const QString &sender, const QString &message)
{
  send(OpcodeMessage, sender, message);
}


/** [public slots]
 * 
 */
void DaemonService::readyRead()
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
    
    if (m_opcode != 401)
      qDebug() << "op" << m_opcode;
    
    if (m_accepted) {
      switch (m_opcode) {
        case OpcodeMessage:
          opcodeMessage();
          break;
          
        case OpcodePong:
          opcodePong();
          break;
          
        case OpcodeNewProfile:
          opcodeNewProfile();
          break;
          
        case OpcodeByeMsg:
          opcodeByeMsg();
          break;
          
        case OpcodeRelayMessage:
          opcodeRelayMessage();
          break;
          
        default:
          unknownOpcode();
          break;
      };
    }
    else if (m_opcode == OpcodeGreeting) {
      if (!opcodeGreeting()) {
        m_socket->disconnectFromHost();
        return;
      }
    }
    else {
      m_socket->disconnectFromHost();
      return;
    }
  }
}


/** [public slots]
 * 
 */
void DaemonService::sendLinkLeave(quint8 numeric, const QString &network, const QString &ip)
{
  send(OpcodeLinkLeave, numeric, network, ip);
}


/** [public slots]
 * 
 */
void DaemonService::sendNewLink(quint8 numeric, const QString &network, const QString &ip)
{
#ifdef SCHAT_DEBUG
  qDebug() << "DaemonService::sendNewLink()" << numeric << network << ip;
#endif
  
  if (m_flag == FlagLink && numeric == m_numeric)
    return;
  
  send(OpcodeNewLink, numeric, network, ip);
}


/** [public slots]
 * 
 */
void DaemonService::sendNewNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name)
{
#ifdef SCHAT_DEBUG
  qDebug() << "DaemonService::sendNewNick()";
#endif
  
  if (m_profile->nick() == nick) {
    m_profile->setGender(gender);
    m_profile->setNick(newNick);
    m_profile->setFullName(name);
  }
  send(OpcodeNewNick, gender, nick, newNick, name);
}


/** [public slots]
 * 
 */
void DaemonService::sendNewProfile(quint8 gender, const QString &nick, const QString &name)
{
#ifdef SCHAT_DEBUG
  qDebug() << "DaemonService::sendNewProfile()";
#endif
  
  if (m_profile->nick() == nick) {
    m_profile->setGender(gender);
    m_profile->setFullName(name);
  }
  send(OpcodeNewProfile, gender, nick, name);
}


/** [public slots]
 * 
 */
void DaemonService::sendRelayMessage(const QString &channel, const QString &sender, const QString &message, quint8 numeric)
{
  if (numeric == m_numeric)
    return;

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
  }
}


/** [private slots]
 * Если `m_accepted` не равен `true`, разрываем соединение,
 * т.к. не произошло рукопожатия за отведённое время.
 */
void DaemonService::ping()
{ 
  if (m_accepted) {
    if (m_pings < 1) {
      send(OpcodePing);
      ++m_pings;
    }
    else
      m_socket->disconnectFromHost();
  }
  else {
    if (m_socket->state() == QAbstractSocket::ConnectedState)
      m_socket->disconnectFromHost();
    else
      deleteLater();
  }
}


/** [private]
 * 
 */
bool DaemonService::opcodeGreeting()
{
#ifdef SCHAT_DEBUG
  qDebug() << "DaemonService::opcodeGreeting()";
#endif

  quint16 p_version;
  quint8  p_gender;
  QString p_nick;
  QString p_name;
  QString p_userAgent;
  QString p_byeMsg;
  quint16 err;

  m_stream >> p_version >> m_flag >> p_gender >> p_nick >> p_name >> p_userAgent >> p_byeMsg;
  m_nextBlockSize = 0;

  QStringList profile;
  profile << p_nick << p_name << p_byeMsg << p_userAgent << m_socket->peerAddress().toString() << AbstractProfile::gender(p_gender);; 
  m_profile = new AbstractProfile(profile, this);

  err = verifyGreeting(p_version);

  if (err) {
    accessDenied(err);
    return false;
  }

  emit greeting(m_profile->pack(), m_flag);

  return true;
}


/** [private]
 * Отправка стандартного пакета:
 * quint16 -> размер пакета
 * quint16 -> опкод
 * ОПКОДЫ:
 *   `OpcodePing`.
 */
bool DaemonService::send(quint16 opcode)
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
 *   `OpcodeServerMessage`.
 */
bool DaemonService::send(quint16 opcode, const QString &msg)
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
 * QString ->
 * QString ->
 * ОПКОДЫ:
 *   `OpcodeMessage`.
 */
bool DaemonService::send(quint16 opcode, const QString &str1, const QString &str2)
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
 * quint16 ->
 * ОПКОДЫ:
 *   `OpcodeAccessGranted`, `OpcodeAccessDenied`.
 */
bool DaemonService::send(quint16 opcode, quint16 err)
{
  if (m_socket->state() == QTcpSocket::ConnectedState) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode << err; 
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
 *   `OpcodePrivateMessage`.
 */
bool DaemonService::send(quint16 opcode, quint8 flag, const QString &nick, const QString &message)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode << flag << nick << message; 
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
 * QString ->
 * ОПКОДЫ:
 *   `OpcodeNewNick`.
 */
bool DaemonService::send(quint16 opcode, quint8 gender, const QString &nick, const QString &newNick, const QString &name)
{
  if (isReady()) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << opcode << gender << nick << newNick << name; 
    out.device()->seek(0);
    out << quint16(block.size() - (int) sizeof(quint16));
    m_socket->write(block);
    return true;
  }
  else
    return false;
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


/** [private]
 * Разбор пакета с опкодом `OpcodeByeMsg`.
 */
void DaemonService::opcodeByeMsg()
{
  qDebug() << "DaemonService::opcodeByeMsg()";
  
  QString p_bye;
  m_stream >> p_bye;
  m_nextBlockSize = 0;
  m_profile->setByeMsg(p_bye);
  emit newBye(m_profile->nick(), p_bye);
}


/** [private]
 * Разбор пакета с опкодом `OpcodeMessage`.
 * В конце разбора высылается сигнал `message(const QString &, const QString &, const QString &)`.
 */
void DaemonService::opcodeMessage()
{
  QString p_channel;
  QString p_message;
  m_stream >> p_channel >> p_message;
  m_nextBlockSize = 0;
#ifdef SCHAT_DEBUG
  qDebug() << "DaemonService::opcodeMessage()";
  qDebug() << "  CHANNEL:" << p_channel;
  qDebug() << "  SENDER: " << m_profile->nick();
  qDebug() << "  MESSAGE:" << p_message;
#endif
  emit message(p_channel, m_profile->nick(), p_message);
}


/** [private]
 * 
 */
void DaemonService::opcodeNewProfile()
{
  qDebug() << "DaemonService::opcodeNewProfile()";
  
  quint8 p_gender;
  QString p_nick;
  QString p_name;
  m_stream >> p_gender >> p_nick >> p_name;
  m_nextBlockSize = 0;
  
  if (m_profile->nick() == p_nick)
    emit newProfile(p_gender, m_profile->nick(), p_name);
  else
    emit newNick(p_gender, m_profile->nick(), p_nick, p_name);
}


/** [private]
 * Разбор пакета с опкодом `OpcodePong`.
 * Функция сбрасывает счётчик `OpcodePong`.
 */
void DaemonService::opcodePong()
{
  m_nextBlockSize = 0;
  m_pings = 0;
}


/** [private]
 * Разбор пакета с опкодом `OpcodeRelayMessage`.
 */
void DaemonService::opcodeRelayMessage()
{
  quint8 p_numeric;
  QString p_channel;
  QString p_sender;
  QString p_message;
  m_stream >> p_numeric >> p_channel >> p_sender >> p_message;
  m_nextBlockSize = 0;
#ifdef SCHAT_DEBUG
  qDebug() << "DaemonService::opcodeRelayMessage()";
  qDebug() << "  CHANNEL:" << p_channel;
  qDebug() << "  SENDER: " << p_sender;
  qDebug() << "  MESSAGE:" << p_message;
#endif
  emit relayMessage(p_channel, p_sender, p_message, p_numeric);
}


/** [private]
 * Функция читает пакет с неизвестным опкодом.
 */
void DaemonService::unknownOpcode()
{
  qDebug() << "DaemonService::unknownOpcode()";
  qDebug() << "opcode:" << m_opcode << "size:" << m_nextBlockSize;
  QByteArray block = m_socket->read(m_nextBlockSize - (int) sizeof(quint16));
  m_nextBlockSize = 0;
}
