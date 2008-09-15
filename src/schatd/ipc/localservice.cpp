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

#include "protocol.h"
#include "ipc/localservice.h"

/*!
 * \class LocalService
 * \brief Класс, обслуживающий локальные соединения.
 * 
 * При наличии валидного сокета, инициализируется сокет.
 */

/*!
 * \brief Конструктор класса LocalService.
 */
LocalService::LocalService(QLocalSocket *socket, QObject *parent)
  : QObject(parent), m_socket(socket)
{
  qDebug() << "LocalService::LocalService()";

  if (m_socket) {
    connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(m_socket, SIGNAL(disconnected()), SLOT(disconnected()));
    m_nextBlockSize = 0;
    m_stream.setDevice(m_socket);
    m_stream.setVersion(StreamVersion);
  }
  else
    deleteLater();
}


void LocalService::disconnected()
{
  qDebug() << "LocalService::disconnected()";
  deleteLater();
}


void LocalService::readyRead()
{
  qDebug() << "LocalService::readyRead()";
}
