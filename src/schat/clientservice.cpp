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


/** [public]
 * 
 */
ClientService::ClientService(const QStringList &profile, const QString &server, quint16 port, QObject *parent)
  : QObject(parent), m_server(server), m_port(port)
{
  m_profile = new AbstractProfile(profile, this);
  m_socket = 0;
}


/** [public]
 * 
 */
ClientService::~ClientService()
{
  qDebug() << "ClientService::~ClientService()";
}


/** [public]
 * Подключение к хосту, в качестве адреса сервера используются
 * приватные мемберы `m_server` и `m_port`.
 * В случае попытки подключения высылается сигнал `void connecting(const QString &server)`.
 */
void ClientService::connectToHost()
{
  qDebug() << "ClientService::connectToHost()";
  
  if (!m_socket)
    createSocket();
  
  if (m_socket->state() == QAbstractSocket::UnconnectedState) {
    m_socket->connectToHost(m_server, m_port);
    emit connecting(m_server);
  }
}


/** [private slots]
 * Слот вызывается при успешном подключении сокета `m_socket` к серверу.
 * Слот отправляет приветственное сообщение серверу (OpcodeGreeting).
 */
void ClientService::connected()
{
  qDebug() << "ClientService::connected()";
  
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
 */
void ClientService::disconnected()
{
  qDebug() << "ClientService::disconnected()";
  if (m_socket) {
    m_socket->deleteLater();
    m_socket = 0;
  }
}


/** [private slots]
 * Слот вызывается когда поступила новая порция данных для чтения из сокета `m_socket`.
 */
void ClientService::readyRead()
{
  qDebug() << "ClientService::readyRead()";
  
}


/** [private]
 * Функция создаёт сокет `m_socket` и создаёт необходимые соединения сигнал-слот.
 * ВНИМАНИЕ: функция не проверяет наличие сокета `m_socket`, это должно делаться за пределами функции.
 */
void ClientService::createSocket()
{
  m_socket = new QTcpSocket(this);
  connect(m_socket, SIGNAL(connected()), SLOT(connected()));
  connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
  connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()));
}
