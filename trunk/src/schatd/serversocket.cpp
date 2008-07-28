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
  m_state = sChatStateWaitingForGreeting;
  m_stream.setDevice(this);
  m_stream.setVersion(StreamVersion);
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
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(StreamVersion);
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
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(StreamVersion);
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
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(StreamVersion);
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
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(StreamVersion);
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
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(StreamVersion);
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
    if (m_command != OpcodeGreeting) {
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
      case OpcodeMessage:
        opSendMessage();
        break;
        
      case OpcodeNewProfile: 
        clientSendNewProfile();
        break;
      
      case OpcodePong:
        m_failurePongs = 0;
        break;
      
      // Опкод `sChatOpcodePing`
      // В ответ отсылаем `sChatOpcodePong`
      case OpcodePing:
        send(OpcodePong);
        break;
      
      // Опкод `sChatOpcodeClientQuit`
      // Клиент выходит из часа
//      case sChatOpcodeClientQuit:
//        abort();
//        return;
      
      // Опкод `sChatOpcodeSendByeMsg`
      // Клиент отправил текст сообщения при выходе
      case sChatOpcodeSendByeMsg:
        opSendByeMsg();
        break;
        
      case sChatOpcodeGetServerInfo:
        opGetServerInfo();
        break;
      
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
    send(OpcodePing);
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
quint16 ServerSocket::verifyGreeting(quint16 version)
{
  if (version < ProtocolVersion)
    return ErrorOldClientProtocol;
  else if (version > ProtocolVersion)
    return ErrorOldServerProtocol;
  else if (!(m_flag == FlagNone || m_flag == FlagDirect))
    return ErrorBadGreetingFlag;
  else if (!m_profile->isValidNick())
    return ErrorBadNickName;
  else if (!m_profile->isValidUserAgent())
    return ErrorBadUserAgent;
  
  #ifndef SCHAT_CLIENT
  if (m_flag == FlagDirect) {
    return sChatErrorDirectNotAllow;
    m_profile->setNick("#DUBLICATE");
  }
  #endif
  
  return 0;
}


/** [private]
 * 
 */
void ServerSocket::clientSendNewProfile()
{
  quint16 Sex;
  QString Nick;
  QString Name;
  
  m_stream >> Sex >> Nick >> Name;
  
  if (Profile::isValidNick(Nick))
    emit clientSendNewProfile(Sex, Nick, Name);
}


/** [private]
 * 
 */
void ServerSocket::opGetServerInfo()
{
  QString info = QString("<b><a href='http://impomezia.net.ru'>IMPOMEZIA</a> Simple Chat Daemon %1</b>, ").arg(SCHAT_VERSION);
  
#if   defined(Q_OS_AIX)
  info += "AIX";
#elif defined(Q_OS_BSD4)
  info += "BSD 4.4 ";
#elif defined(Q_OS_BSDI)
  info += "BSD/OS";
#elif defined(Q_OS_CYGWIN)
  info += "Cygwin";
#elif defined(Q_OS_DARWIN)
  info += "Darwin OS";
#elif defined(Q_OS_DGUX)
  info += "DG/UX";
#elif defined(Q_OS_DYNIX)
  info += "DYNIX/ptx";
#elif defined(Q_OS_FREEBSD)
  info += "FreeBSD";
#elif defined(Q_OS_HPUX)
  info += "HP-UX";
#elif defined(Q_OS_HURD)
  info += "GNU Hurd";
#elif defined(Q_OS_IRIX)
  info += "SGI Irix";
#elif defined(Q_OS_LINUX)
  info += "Linux";
#elif defined(Q_OS_LYNX)
  info += "LynxOS";
#elif defined(Q_OS_MSDOS)
  info += "Windows";
#elif defined(Q_OS_NETBSD)
  info += "NetBSD";
#elif defined(Q_OS_OS2)
  info += "OS/2";
#elif defined(Q_OS_OPENBSD)
  info += "OpenBSD";
#elif defined(Q_OS_OS2EMX)
  info += "XFree86 on OS/2 (not PM)";
#elif defined(Q_OS_OSF)
  info += "HP Tru64 UNIX";
#elif defined(Q_OS_QNX6)
  info += "QNX RTP 6.1";
#elif defined(Q_OS_QNX)
  info += "QNX";
#elif defined(Q_OS_RELIANT)
  info += "Reliant UNIX";
#elif defined(Q_OS_SCO)
  info += "SCO OpenServer 5";
#elif defined(Q_OS_SOLARIS)
  info += "Sun Solaris";
#elif defined(Q_OS_ULTRIX)
  info += "DEC Ultrix";
#elif defined(Q_OS_UNIXWARE)
  info += "UnixWare";
#elif defined(Q_OS_WIN32)
  info += "Windows";
#elif defined(Q_OS_WINCE)
  info += "Windows CE";
#endif
  send(sChatOpcodeServerInfo, info);
}


/** [private]
 * Опкод `sChatOpcodeSendByeMsg`
 */
void ServerSocket::opSendByeMsg()
{
  QString ByeMsg;
  m_stream >> ByeMsg;
  m_profile->setByeMsg(ByeMsg);
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
  QString fullName;
  QString nick;
  QString userAgent;
  quint16 err = 0;
  quint8  sex;
  quint16 version;
  
  m_stream >> version >> m_flag >> sex >> nick >> fullName >> userAgent;
  
  m_profile = new Profile(nick, fullName, sex, this);
  m_profile->setUserAgent(userAgent);
  m_profile->setHost(peerAddress().toString());
  
  err = verifyGreeting(version);
  
  if (err) {
    send(OpcodeAccessDenied, err);
    disconnectFromHost();
    return;
  }
  
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
