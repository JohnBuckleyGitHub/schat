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

#include "abstractprofile.h"
#include "clientservice.h"
#include "protocol.h"


static const int CheckTimeout         = 6000;
static const int ReconnectTimeout     = 3000;
static const int ReconnectFailTimeout = 20000;


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
  
  connect(&m_checkTimer, SIGNAL(timeout()), SLOT(check()));
  connect(&m_reconnectTimer, SIGNAL(timeout()), SLOT(reconnect()));
}


/** [public]
 * 
 */
ClientService::~ClientService()
{
  qDebug() << "ClientService::~ClientService()";
}


/** [public]
 * Подключение к хосту, за выдачу адреса сервера и порта отвечает класс `m_network`.
 * В случае попытки подключения высылается сигнал `void connecting(const QString &, bool)`.
 */
void ClientService::connectToHost()
{
  qDebug() << "ClientService::connectToHost()";
  
  if (!m_socket)
    createSocket();
  
  if (m_socket->state() == QAbstractSocket::UnconnectedState) {
    m_server = m_network->server();
    m_socket->connectToHost(m_server.address, m_server.port);
    if (m_network->isNetwork())
      emit connecting(m_network->name(), true);
    else
      emit connecting(m_server.address, false);
    
    m_checkTimer.start();
  }
}


/** [private slots]
 * Разрыв соединения или переподключение если после `CheckTimeout` миллисекунд не удалось установить действующие соединение.
 */
void ClientService::check()
{
  qDebug() << "ClientService::check()";
  if (m_socket)
    qDebug() << m_socket->state();
  
  if (m_socket && m_socket->state() != QTcpSocket::ConnectedState) {
    m_socket->deleteLater();
    m_socket = 0;
    connectToHost();
  }
  else if (!m_accepted && m_socket->state() == QTcpSocket::ConnectedState)
    m_socket->disconnectFromHost();
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
  qDebug() << "ClientService::connected()";
  
  m_nextBlockSize = 0;
  m_reconnectTimer.stop();
  m_reconnectTimer.setInterval(ReconnectTimeout);
  
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(StreamVersion);
  out << quint16(0)
      << OpcodeGreeting
      << ProtocolVersion
      << FlagNone
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
  qDebug() << "ClientService::disconnected()";
  if (m_socket) {
    m_socket->deleteLater();
    m_socket = 0;
  }
  emit unconnected();
  m_accepted = false;
  
  if (!m_fatal) {
    if ((m_reconnects < m_network->count() * 2))
      reconnect();

    m_reconnectTimer.start();
  }
}


/** [private slots]
 * Слот вызывается когда поступила новая порция данных для чтения из сокета `m_socket`.
 */
void ClientService::readyRead()
{
  qDebug() << "ClientService::readyRead()";
  
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
        case OpcodeNewUser:
          opcodeNewUser();
          break;
          
        default:
          m_socket->disconnectFromHost();
          break;
      };
    }
    else if (m_opcode == OpcodeAccessGranted) {
      opcodeAccessGranted();
    }
    else if (m_opcode == OpcodeAccessDenied) {
      quint16 reason;
      m_stream >> reason;
      m_nextBlockSize = 0;
      qDebug() << "reason" << reason;;
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
  qDebug() << "ClientService::reconnect()" << m_reconnectTimer.interval() << m_reconnects;
  
  if (m_fatal)
    return;
  
  m_reconnects++;
  
  if (!m_socket)
    connectToHost();
}


/** [private]
 * Функция создаёт сокет `m_socket` и создаёт необходимые соединения сигнал-слот.
 * ВНИМАНИЕ: функция не проверяет наличие сокета `m_socket`, это должно делаться за пределами функции.
 */
void ClientService::createSocket()
{
  m_socket = new QTcpSocket(this);
  m_stream.setDevice(m_socket);
  connect(m_socket, SIGNAL(connected()), SLOT(connected()));
  connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
  connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()));
}


/** [private]
 * Обработка опкода `OpcodeAccessGranted`.
 * Функция отправляет сигнал `accessGranted(const QString &, const QString &, quint16)`.
 * Если установлено подключение к одиночному серверу, то имя сети устанавливается "".
 */
void ClientService::opcodeAccessGranted()
{
  quint16 level;
  m_stream >> level;
  m_nextBlockSize = 0;
  m_accepted = true;
  m_reconnects = 0;
  
  QString network;
  if (m_network->isNetwork())
    network = m_network->name();
  else
    network = "";
  
  emit accessGranted(network, m_server.address, level);
}


/** [private]
 * Разбор пакета с опкодом `OpcodeNewUser`.
 * В конце разбора высылается сигнал `void newUser(const QStringList &, bool)`.
 */
void ClientService::opcodeNewUser()
{
  qDebug() << "ClientService::opcodeNewUser()";
  bool echo;
  quint8 p_flag;
  quint8 p_gender;
  QString p_nick;
  QString p_name;
  QString p_agent;
  QString p_host;
  
  m_stream >> p_flag >> p_gender >> p_nick >> p_name >> p_agent >> p_host;
  m_nextBlockSize = 0;
  QStringList profile;
  profile << p_nick << p_name << "" << p_agent << p_host << AbstractProfile::gender(p_gender);
  
  if (p_nick == m_profile->nick() || p_flag == 0)
    echo = false;
  else
    echo = true;

  emit newUser(profile, echo);  
}
