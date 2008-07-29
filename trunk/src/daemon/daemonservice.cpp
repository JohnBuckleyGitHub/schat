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
 * 
 */
void DaemonService::sendServerMessage(const QString &msg)
{
  send(OpcodeServerMessage, msg);
}


/** [public slots]
 * Формирует и отправляет пакет с опкодом `OpcodeNewUser`.
 */
bool DaemonService::newUser(const QStringList &list, bool echo)
{
  qDebug() << "DaemonService::newUser(const QStringList &)" << list.at(AbstractProfile::Nick);
  
  if (!m_accepted)
    return false;
  
  if (m_socket->state() == QTcpSocket::ConnectedState) {
    AbstractProfile profile(list);
    
    if (profile.nick() == m_profile->nick() && !echo)
      return true;
    
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(StreamVersion);
    out << quint16(0) << OpcodeNewUser;

    if (echo)
      out << quint8(1);
    else
      out << quint8(0);
    
    out << profile.genderNum()
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
    emit leave(m_profile->nick());
  
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
  qDebug() << "DaemonService::readyRead()";
  
  forever {
    if (!m_nextBlockSize) {
      if (m_socket->bytesAvailable() < (int) sizeof(quint16))
        break;
        
      m_stream >> m_nextBlockSize;
    }

    if (m_socket->bytesAvailable() < m_nextBlockSize)
      break;
    
    m_stream >> m_opcode;
    
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
void DaemonService::sendNewNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name)
{
  qDebug() << "DaemonService::sendNewNick()";
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
  qDebug() << "DaemonService::sendNewProfile()";
  if (m_profile->nick() == nick) {
    m_profile->setGender(gender);
    m_profile->setFullName(name);
  }
  send(OpcodeNewProfile, gender, nick, name);
}


/** [private slots]
 * Если `m_accepted` не равен `true`, разрываем соединение,
 * т.к. не произошло рукопожатия за отведённое время.
 */
void DaemonService::ping()
{
  qDebug() << "DaemonService::ping()";
  
  if (m_accepted) {
    qDebug() << "m_pings = " << m_pings;
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
  qDebug() << "DaemonService::opcodeGreeting()";
  
  QString f_fullName;
  QString f_nick;
  QString f_userAgent;
  QString f_byeMsg;
  quint16 f_err = 0;
  quint8  f_sex;
  quint16 f_version;
  
  m_stream >> f_version >> m_flag >> f_sex >> f_nick >> f_fullName >> f_userAgent >> f_byeMsg;
  m_nextBlockSize = 0;
  
  QStringList profile;
  profile << f_nick << f_fullName << f_byeMsg << f_userAgent << m_socket->peerAddress().toString() << AbstractProfile::gender(f_sex); 
  m_profile = new AbstractProfile(profile, this);
  
  f_err = verifyGreeting(f_version);
  
  if (f_err) {
    accessDenied(f_err);
    return false;
  }
  
  // При прямом подключении `sChatFlagDirect` не проверяем имя на дубликаты
  // и не отсылаем его в общий список.
  #ifdef SCHAT_CLIENT
  if (m_flag == sChatFlagDirect)
    emit appendDirectParticipant(m_profile->nick());
  else
  #endif
  emit greeting(m_profile->pack());
  
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
 * 
 */
quint16 DaemonService::verifyGreeting(quint16 version)
{
  if (version < ProtocolVersion)
    return ErrorOldClientProtocol;
  else if (version > ProtocolVersion)
    return ErrorOldServerProtocol;
  else if (m_flag != FlagNone)
    return ErrorBadGreetingFlag;
  else if (!m_profile->isValidNick())
    return ErrorBadNickName;
  else if (!m_profile->isValidUserAgent())
    return ErrorBadUserAgent;
  
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
  qDebug() << "DaemonService::opcodeMessage()";
  QString p_channel;
  QString p_message;
  m_stream >> p_channel >> p_message;
  m_nextBlockSize = 0;
  qDebug() << "CHANNEL:" << p_channel;
  qDebug() << "SENDER: " << m_profile->nick();
  qDebug() << "MESSAGE:" << p_message;
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
 * Функция читает пакет с неизвестным опкодом.
 */
void DaemonService::unknownOpcode()
{
  qDebug() << "DaemonService::unknownOpcode()";
  qDebug() << "opcode:" << m_opcode << "size:" << m_nextBlockSize;
  QByteArray block = m_socket->read(m_nextBlockSize - (int) sizeof(quint16));
  m_nextBlockSize = 0;
}
