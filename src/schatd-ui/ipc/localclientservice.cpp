/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.com)
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

#include "localclientservice.h"
#include "protocol.h"

/*!
 * \class LocalClientService
 * \brief Универсальный сервис клиента чата.
 */

/*!
 * \brief Конструктор класса LocalClientService.
 */
LocalClientService::LocalClientService(QObject *parent)
  : QObject(parent)
{
  m_nextBlockSize = 0;
  m_socket = new QLocalSocket(this);
  m_stream.setVersion(StreamVersion);
  m_stream.setDevice(m_socket);
  connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
  connect(m_socket, SIGNAL(error(QLocalSocket::LocalSocketError)), SLOT(error(QLocalSocket::LocalSocketError)));
  connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()));
}


void LocalClientService::connectToServer()
{
  qDebug() << "LocalClientService::connectToServer()";

  m_nextBlockSize = 0;
  m_socket->abort();
  m_socket->connectToServer("fortune");
}


void LocalClientService::disconnected()
{
  qDebug() << "LocalClientService::disconnected()";
  emit notify(Stop);
}


void LocalClientService::error(QLocalSocket::LocalSocketError err)
{
  qDebug() << "LocalClientService::error()" << err;
}


void LocalClientService::readyRead()
{
  qDebug() << "LocalClientService::readyRead()";
  emit notify(Stop);
}
