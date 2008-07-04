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
#include <stdlib.h>

#include "serversocket.h"
#include "version.h"
#include "profile.h"


/** [public]
 * 
 */
ServerSocket::ServerSocket(QObject *parent)
  : QTcpSocket(parent)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "ServerSocket::ServerSocket(QObject *parent)";
  #endif
  
  m_state = sChatStateWaitingForGreeting;
  m_stream.setDevice(this);
  m_stream.setVersion(sChatStreamVersion);
  m_failurePongs = 0;
  
  #ifdef SCHAT_CLIENT
  connect(this, SIGNAL(appendDirectParticipant(const QString &)), parent, SLOT(appendDirectParticipant(const QString &)));
  #endif
  
  connect(this, SIGNAL(appendParticipant(const QString &)), parent, SLOT(appendParticipant(const QString &)));
  connect(this, SIGNAL(relayMessage(const QString &, const QString &, const QString &)), parent, SLOT(relayMessage(const QString &, const QString &, const QString &)));
  
  connect(this, SIGNAL(disconnected()), &m_pingTimer, SLOT(stop()));
  connect(this, SIGNAL(disconnected()), parent, SLOT(disconnected()));
  connect(this, SIGNAL(readyRead()), this, SLOT(readyRead()));
  connect(&m_pingTimer, SIGNAL(timeout()), this, SLOT(sendPing()));
  connect(this, SIGNAL(error(QAbstractSocket::SocketError)), parent, SLOT(connectionError(QAbstractSocket::SocketError)));
  connect(this, SIGNAL(clientSendNewProfile(quint16, const QString &, const QString &)), parent, SLOT(clientSendNewProfile(quint16, const QString &, const QString &)));

  m_nextBlockSize = 0;
  m_pingTimer.setInterval(PingInterval);
}


/** [public]
 * ОПКОДЫ:
 *   sChatOpcodeGreetingOk
 */
void ServerSocket::send(quint16 opcode)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void ServerSocket::send(quint16 opcode)" << opcode;
  #endif
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode;
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));      
  write(block);
}


/** [public]
 * ОПКОДЫ:
 *   sChatOpcodeSendPrivateMessage
 */
void ServerSocket::send(quint16 opcode, const QString &n, const QString &m)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void ServerSocket::send(quint16 opcode, const QString &n, const QString &m)" << opcode;
  #endif
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode << n << m;  
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));      
  write(block);  
}


/** [public]
 * ОПКОДЫ:
 *   sChatOpcodeParticipantLeft
 */
void ServerSocket::send(quint16 opcode, const QString &s)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void ServerSocket::send(quint16 opcode, const QString &s)" << opcode;
  #endif
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode << s;  
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));      
  write(block);  
}


/** [public]
 * ОПКОДЫ:
 *   sChatOpcodeError
 *   sChatOpcodeMaxDoublePingTimeout
 */
void ServerSocket::send(quint16 opcode, quint16 err)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void ServerSocket::send(quint16 opcode, const QString &s)" << opcode << err;
  #endif
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode << err; 
  out.device()->seek(0);
  out << quint16(block.size() - sizeof(quint16));
  write(block);  
}


/** [public]
 * ОПКОДЫ:
 *   sChatOpcodeNewParticipant
 *   sChatOpcodeNewParticipantQuiet
 */
void ServerSocket::send(quint16 opcode, quint16 s, const QStringList &list)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void ServerSocket::send(quint16 opcode, quint16 s, const QStringList &list)" << opcode << s << list;
  #endif
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode << s;
  
  foreach (QString str, list)
    out << str;
  
  out.device()->seek(0);
  out << quint16(block.size() - 2);      
  write(block);  
}


/** [private slots]
 * Слот вызывается сигналом `readyRead()`
 * т.е. слот вызывается тогда когда сокет готов
 * прочитать новые данные. 
 */
void ServerSocket::readyRead()
{
  // Состояние `sChatStateWaitingForGreeting`
  // Ожидаем пакет с опкодом `sChatOpcodeGreeting`
  // Если пришёл другой пакет, рвём соединение `abort()`
  // Если получен нужный пакет устанавливаем состояние `sChatStateReadingGreeting`
  if (m_state == sChatStateWaitingForGreeting) {
    if (!readBlock())
      return;
    if (m_command != sChatOpcodeGreeting) {
      abort();
      return;
    }
    m_state = sChatStateReadingGreeting;
  }
  
  // Состояние `sChatStateReadingGreeting`
  // Вызываем функцию `readGreeting()` для чтения пакета `sChatOpcodeGreeting`
  if (m_state == sChatStateReadingGreeting)
    readGreeting();
  
  while (readBlock()) {
    
    switch (m_command) {
      case sChatOpcodeSendMessage:
        opSendMessage();
        break;
        
      case sChatOpcodeNewProfile:
        clientSendNewProfile();
        break;
      
      case sChatOpcodePong:
        m_failurePongs = 0;
        break;
      
      // Опкод `sChatOpcodePing`
      // В ответ отсылаем `sChatOpcodePong`
      case sChatOpcodePing:
        send(sChatOpcodePong);
        break;
      
      // Опкод `sChatOpcodeClientQuit`
      // Клиент выходит из часа
      case sChatOpcodeClientQuit:
        abort();
        return;
        
      default:
        #ifdef SCHAT_DEBUG
        qDebug() << "Invalid Opcode";
        #endif
        abort();
        return;
    }
  }
  
  // Запускаем в случае необходимости ping таймер
  if (!m_pingTimer.isActive())
    m_pingTimer.start();
}


/** [private slots]
 * Отправляем пакет с опкодом `sChatOpcodePing`
 * и увеличиваем счётчик `m_failurePongs` на 1
 * ответный пакет `sChatOpcodePong` сбрасывает этот счётчик.
 * В случае если отправка двух пакетов завершилась неудачей,
 * т.е. не получено подтверждение `sChatOpcodePong`,
 * то разрываем соединение.
 */
void ServerSocket::sendPing()
{
  if (m_failurePongs < 2) {
    send(sChatOpcodePing);
    ++m_failurePongs;
  }
  else
    abort();
}


/** [private]
 * Читаем блок данных
 */
bool ServerSocket::readBlock()
{
  if (m_nextBlockSize == 0) {
    if (bytesAvailable() < 2)
      return false;
    m_stream >> m_nextBlockSize;
  }

  if (bytesAvailable() < m_nextBlockSize)
    return false;

  m_stream >> m_command;
  
  m_nextBlockSize = 0;
  return true;
}


/** [private]
 * 
 */
void ServerSocket::clientSendNewProfile()
{
  quint16 sex;
  QString nick;
  QString name;
  
  m_stream >> sex >> nick >> name;
  
  if (Profile::isValidNick(nick))
    emit clientSendNewProfile(sex, nick, name);
}


/** [private]
 * Опкод `sChatOpcodeSendMessage`
 */
void ServerSocket::opSendMessage()
{
  QString channel;
  QString message;
  
  m_stream >> channel >> message;
  
  if (channel.isEmpty() || message.isEmpty()) {
    abort();
    return;
  }
  
  #ifdef SCHAT_CLIENT
  if (m_flag == sChatFlagDirect) {
    send(sChatOpcodeSendPrvMessageEcho, channel, message);
    emit newMessage(m_profile->nick(), message);
  }
  else
  #endif
    emit relayMessage(channel, m_profile->nick(), message);
}


/** [private]
 * Функция для чтения пакета `sChatOpcodeGreeting`
 * Проверяем корректность данных, в случае ошибки
 * посылаем пакет `sChatOpcodeError`
 * и рвём соединение `disconnectFromHost()`
 * 
 * В случае успеха вызываем сигнал `appendParticipant(const QString &p)`
 * и устанавливаем состояние `sChatStateWaitingForChecking`
 * т.е. сервер должен проверить имя на дубликат.
 */
void ServerSocket::readGreeting()
{
  QString FullName;
  QString Nick;
  QString UserAgent;
  quint16 err = 0;
  quint8 Sex;
  quint8 version;
  
  m_stream >> version >> m_flag >> Sex >> Nick >> FullName >> UserAgent;
  
  m_profile = new Profile(Nick, FullName, Sex, this);
  m_profile->setUserAgent(UserAgent);
  m_profile->setHost(peerAddress().toString());
  
  if (version != sChatProtocolVersion)
    err = sChatErrorBadProtocolVersion;
  else if (!(m_flag == sChatFlagNone || m_flag == sChatFlagDirect))
    err = sChatErrorBadGreetingFlag;
  else if (!m_profile->isValidNick())
    err = sChatErrorBadNickName;
  else if (!m_profile->isValidUserAgent())
    err = sChatErrorBadUserAgent;
  else if (!isValid())
    err = sChatErrorInvalidConnection;
  
  #ifndef SCHAT_CLIENT
  if (m_flag == sChatFlagDirect) {
    err = sChatErrorDirectNotAllow;
    m_profile->setNick("#DUBLICATE");
  }
  #endif
  
  if (err) {
    send(sChatOpcodeError, err);
    disconnectFromHost();
    return;
  }
  
  #ifdef SCHAT_DEBUG
//  qDebug() << (nick + "@" + peerAddress().toString() + ":" + QString::number(peerPort())) << userAgent;
  #endif
  
  // При прямом подключении `sChatFlagDirect` не проверяем имя на дубликаты
  // и не отсылаем его в общий список.
  #ifdef SCHAT_CLIENT
  if (m_flag == sChatFlagDirect)
    emit appendDirectParticipant(m_profile->nick());
  else
  #endif
    emit appendParticipant(m_profile->nick());
  
  m_state = sChatStateWaitingForChecking;
}


/** [private]
 * 
 */
#ifdef SCHAT_CLIENT
void ServerSocket::sendLocalProfile()
{
  localProfile->setHost(localAddress().toString());  
  send(sChatOpcodeNewParticipantQuiet, localProfile->sex(), localProfile->toList());  
}
#endif
