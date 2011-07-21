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

#include "Core.h"
#include "debugstream.h"
#include "events.h"
#include "net/SimpleSocket.h"
#include "Server.h"
#include "Worker.h"

Worker::Worker(int id, Core *parent)
  : QObject(0),
    m_id(id),
    m_core(parent),
    m_nextSocketId(0)
{
}

Worker::~Worker()
{
  m_server->close();
  delete m_server;
}


void Worker::setDefaultSslConf(const QString &crtFile, const QString &keyFile)
{
  #if !defined(SCHAT_NO_SSL)
  if (!QSslSocket::supportsSsl())
    return;

  if (crtFile.isEmpty() || keyFile.isEmpty())
    return;

  QSslConfiguration conf = QSslConfiguration::defaultConfiguration();

  QFile file(crtFile);
  if (file.open(QIODevice::ReadOnly)) {
    conf.setLocalCertificate(QSslCertificate(&file));
    file.close();
  }

  file.setFileName(keyFile);
  if (file.open(QIODevice::ReadOnly)) {
    conf.setPrivateKey(QSslKey(&file, QSsl::Rsa));
    file.close();
  }

  QSslConfiguration::setDefaultConfiguration(conf);
  #else
  Q_UNUSED(crtFile)
  Q_UNUSED(keyFile)
  #endif
}


bool Worker::start()
{
  m_server = new Server;
  connect(m_server, SIGNAL(newConnection(int)), SLOT(newConnection(int)), Qt::DirectConnection);

  return m_server->listen(QHostAddress::Any, 7667);
}


/*!
 * Обработка событий.
 */
void Worker::customEvent(QEvent *event)
{
  SCHAT_DEBUG_STREAM(this << "customEvent()" << event->type() << QThread::currentThread())

  int type = event->type();

  // Отправка пакетов.
  if (type == ServerEvent::NewPackets) {

    NewPacketsEvent *e = static_cast<NewPacketsEvent*>(event);

    // Обработка отправки пакетов нескольким отправителям.
    if (e->workerId() == -1) {
      for (int i = 0; i < e->socketIds.size(); ++i) {
        SimpleSocket *socket = m_sockets.value(e->socketIds.at(i));
        if (!socket)
          continue;

        socket->setTimestamp(e->timestamp);
        socket->send(e->packets);
      }
      return;
    }

    // Обработка отправки пакетов одному отправителю.
    SimpleSocket *socket = m_sockets.value(e->socketId());
    if (!socket)
      return;

    socket->setTimestamp(e->timestamp);
    socket->send(e->packets);
    if (e->option) {
      if (e->option == NewPacketsEvent::KillSocketOption)
        socket->leave();
      else if (e->option == NewPacketsEvent::AuthorizeSocketOption)
        socket->setAuthorized(e->userId());
    }
  }
}


void Worker::newConnection(int socketDescriptor)
{
  SCHAT_DEBUG_STREAM(this << "newConnection()" << "id:" << m_nextSocketId)

  SimpleSocket *socket = new SimpleSocket(m_server);
  socket->setId(m_nextSocketId);
  if (socket->setSocketDescriptor(socketDescriptor)) {
    connect(socket, SIGNAL(newPackets(quint64, const QList<QByteArray> &)), SLOT(newPackets(quint64, const QList<QByteArray> &)), Qt::DirectConnection);
    connect(socket, SIGNAL(released(quint64)), SLOT(released(quint64)), Qt::DirectConnection);
    m_sockets.insert(m_nextSocketId, socket);
    ++m_nextSocketId;
  }
  else
    socket->deleteLater();
}


/*!
 * Обработка новых виртуальных пакетов.
 */
void Worker::newPackets(quint64 id, const QList<QByteArray> &packets)
{
  NewPacketsEvent *event = new NewPacketsEvent(m_id, id, packets, m_sockets.value(id)->userId());

  SimpleSocket *socket = m_sockets.value(id);
  if (socket && !socket->isAuthorized())
    event->address = socket->peerAddress();

  QCoreApplication::postEvent(m_core, event);
}


void Worker::released(quint64 id)
{
  SCHAT_DEBUG_STREAM(this << "released()" << id)

  SimpleSocket *socket = m_sockets.value(id);
  if (socket->isAuthorized()) {
    SocketReleaseEvent *event = new SocketReleaseEvent(m_id, id, socket->errorString(), socket->userId());
    QCoreApplication::postEvent(m_core, event);
  }

  m_sockets.remove(id);
  socket->deleteLater();
}
