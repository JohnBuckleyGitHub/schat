/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QCoreApplication>
#include <QFile>
#include <QSslConfiguration>
#include <QSslKey>
#include <QThread>

#include "debugstream.h"
#include "events.h"
#include "net/SimpleSocket.h"
#include "Server.h"
#include "Worker.h"
#include "WorkerThread.h"

Worker::Worker(WorkerEventListener *listener, QObject *parent)
  : QObject(parent)
  , m_core(listener->core())
  , m_listener(listener)
{
}

Worker::~Worker()
{
  m_server->close();
  delete m_server;
}


bool Worker::start(const QString &listen)
{
  int index = listen.lastIndexOf(QLatin1String(":"));
  if (index == -1)
    return false;

  QHostAddress address = QHostAddress(listen.left(index));
  if (address.isNull())
    return false;

  quint16 port = listen.mid(index + 1).toUInt();
  if (!port)
    return false;

  m_server = new Server;
  connect(m_server, SIGNAL(newConnection(int)), SLOT(newConnection(int)), Qt::DirectConnection);

  return m_server->listen(address, port);
}


void Worker::newConnection(int socketDescriptor)
{
  SCHAT_DEBUG_STREAM(this << "newConnection()" << "id:" << QThread::currentThread())

  SimpleSocket *socket = new SimpleSocket(m_server);
  socket->setId(m_listener->counter());

  if (socket->setSocketDescriptor(socketDescriptor)) {
    connect(socket, SIGNAL(newPackets(quint64, const QList<QByteArray> &)), SLOT(newPackets(quint64, const QList<QByteArray> &)), Qt::DirectConnection);
    connect(socket, SIGNAL(released(quint64)), SLOT(released(quint64)), Qt::DirectConnection);
    m_listener->add(socket);
  }
  else
    socket->deleteLater();
}


/*!
 * Обработка новых виртуальных пакетов.
 */
void Worker::newPackets(quint64 id, const QList<QByteArray> &packets)
{
  SimpleSocket *socket = m_listener->socket(id);
  if (!socket)
    return;

  NewPacketsEvent *event = new NewPacketsEvent(id, packets, socket->userId());

  if (socket && !socket->isAuthorized())
    event->address = socket->peerAddress();

  QCoreApplication::postEvent(m_core, event);
}


void Worker::released(quint64 id)
{
  SCHAT_DEBUG_STREAM(this << "released()" << id)

  SimpleSocket *socket = m_listener->socket(id);
  if (socket && socket->isAuthorized()) {
    SocketReleaseEvent *event = new SocketReleaseEvent(id, socket->errorString(), socket->userId());
    QCoreApplication::postEvent(m_core, event);
  }

  m_listener->release(id);
}
