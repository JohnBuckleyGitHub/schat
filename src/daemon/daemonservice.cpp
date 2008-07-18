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
#include "profile.h"
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
    m_stream.setVersion(sChatStreamVersion);
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
 * ОПКОДЫ:
 *   sChatOpcodeGreetingOk
 */
void DaemonService::send(quint16 opcode)
{
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(sChatStreamVersion);
  out << quint16(0) << opcode;
  out.device()->seek(0);
  out << quint16(block.size() - (int) sizeof(quint16));
  m_socket->write(block);
}


/** [public slots]
 * 
 */
void DaemonService::disconnected()
{
  qDebug() << "DaemonService::disconnected()";
  deleteLater();
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
      ;
    }
    else if (m_opcode == sChatOpcodeGreeting) {
      if (opcodeGreeting())
        m_accepted = true;
      else {
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


/** [private]
 * 
 */
bool DaemonService::opcodeGreeting()
{
  qDebug() << "DaemonService::opcodeGreeting()";
  
  QString f_fullName;
  QString f_nick;
  QString f_userAgent;
  quint16 f_err = 0;
  quint8  f_sex;
  quint16 f_version;
  
  m_stream >> f_version >> m_flag >> f_sex >> f_nick >> f_fullName >> f_userAgent;
  
  m_nextBlockSize = 0;
  
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
quint16 DaemonService::verifyGreeting(quint16 version)
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
