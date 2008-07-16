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

#include <QtNetwork>

#include "daemonthread.h"
#include "profile.h"
#include "protocol.h"


static const int Timeout = 5 * 1000;


/** [public]
 * Конструктор
 */
DaemonThread::DaemonThread(int socketDescriptor, QObject *parent)
  : QThread(parent)
{
  m_descriptor = socketDescriptor;
  m_quit = false;
  m_socket = 0;
  m_state = WaitingGreeting;
  m_nextBlockSize = 0;
}


/** [public]
 * Деструктор
 */
DaemonThread::~DaemonThread()
{
  qDebug() << "DaemonThread::~DaemonThread()";
  if (m_socket)
    delete m_socket;  
}


/** [public]
 * 
 */
void DaemonThread::run()
{
  qDebug() << "DaemonThread::run()";
  
  QTcpSocket soc;
  m_socket = new QTcpSocket;
  m_stream.setDevice(m_socket);
  m_stream.setVersion(sChatStreamVersion);
  
  connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()), Qt::DirectConnection);
  connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()), Qt::DirectConnection);
  
  if (!m_socket->setSocketDescriptor(m_descriptor)) {
    qDebug() << "ERR: NOT SET SOCKET DESCRIPTOR";
//    emit error(m_socket->error());
    return;
  }
  
  if (!m_socket->waitForReadyRead(Timeout)) {
    qDebug() << "ERR: WAIT FOR READY READ TIMEOUT";
//    emit error(m_socket->error());
    return;
  }

  exec();
}


/** [public]
 * ОПКОДЫ:
 *   sChatOpcodeGreetingOk
 */
void DaemonThread::send(quint16 opcode)
{
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode;
  out.device()->seek(0);
  out << quint16(block.size() - (int) sizeof(quint16));
  this->m_socket->write(block);
}


/** [private slot]
 * 
 */
void DaemonThread::disconnected()
{
  qDebug() << "DaemonThread::disconnected()";
  leave();  
}


/** [private slots]
 * Слот вызывается когда есть новые данные для чтения.
 */
void DaemonThread::readyRead()
{
  qDebug() << "DaemonThread::readyRead()";
  
  forever {
    if (!m_nextBlockSize) {
      if (m_socket->bytesAvailable() < (int) sizeof(quint16))
        break;
        
      m_stream >> m_nextBlockSize;
    }

    if (m_socket->bytesAvailable() < m_nextBlockSize)
      break;
    
    m_stream >> m_opcode;
    
    qDebug() << m_opcode;
    
    if (m_state == Accepted) {
      ;
    }
    else if (m_opcode == sChatOpcodeGreeting) {
      if (opcodeGreeting())
        m_state = Accepted;
      else
        leave();
    }
    else
      leave();
  }
}


/** [private]
 * 
 */
bool DaemonThread::opcodeGreeting()
{
  qDebug() << "DaemonThread::opcodeGreeting()";
  
  QString f_fullName;
  QString f_nick;
  QString f_userAgent;
  quint16 f_err = 0;
  quint8  f_sex;
  quint16 f_version;
  
  m_stream >> f_version >> m_flag >> f_sex >> f_nick >> f_fullName >> f_userAgent;
  
  m_profile = new Profile(f_nick, f_fullName, f_sex, this); // TODO оптимизировать
  m_profile->setUserAgent(f_userAgent);
  m_profile->setHost(m_socket->peerAddress().toString());
  
  f_err = verifyGreeting(f_version);
  
  if (f_err) {
//    send(sChatOpcodeError, f_err);
//    disconnectFromHost();
    return false;
  }
  
  // При прямом подключении `sChatFlagDirect` не проверяем имя на дубликаты
  // и не отсылаем его в общий список.
  #ifdef SCHAT_CLIENT
  if (m_flag == sChatFlagDirect)
    emit appendDirectParticipant(m_profile->nick());
  else
  #endif
//    emit appendParticipant(m_profile->nick());
    
  send(sChatOpcodeGreetingOk); // FIXME поместить в положеное место.
  
  return true;
}



/** [private]
 * 
 */
quint16 DaemonThread::verifyGreeting(quint16 version)
{
  if (version < sChatProtocolVersion)
    return sChatErrorOldClientProtocol;
  else if (version > sChatProtocolVersion)
    return sChatErrorOldServerProtocol;
  else if (!(m_flag == sChatFlagNone || m_flag == sChatFlagDirect))
    return sChatErrorBadGreetingFlag;
  else if (!m_profile->isValidNick())
    return sChatErrorBadNickName;
  else if (!m_profile->isValidUserAgent())
    return sChatErrorBadUserAgent;
  else if (!m_socket->isValid())
    return sChatErrorInvalidConnection;
  
  #ifndef SCHAT_CLIENT
  if (m_flag == sChatFlagDirect) {
    return sChatErrorDirectNotAllow;
    m_profile->setNick("#DUBLICATE");
  }
  #endif
  
  return 0;
}


/** [private]
 * 
 */
void DaemonThread::leave()
{
  qDebug() << "DaemonThread::leave()";
  
  if (m_socket->state() == QAbstractSocket::ConnectedState) {
    m_socket->disconnectFromHost();
    m_socket->waitForDisconnected(Timeout);
  }
  exit();
}
