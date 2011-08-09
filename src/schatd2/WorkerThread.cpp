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

#include "debugstream.h"
#include "events.h"
#include "net/SimpleSocket.h"
#include "Worker.h"
#include "WorkerThread.h"

WorkerThread::WorkerThread(const QStringList &listen, QObject *core, QObject *parent)
  : QThread(parent)
  , m_core(core)
  , m_listen(listen)
{
}


void WorkerThread::run()
{
  WorkerEventListener listener(m_core);
//  listener.add("0.0.0.0:7667");
  foreach (QString listen, m_listen) {
    listener.add(listen);
  }

  emit ready(&listener);
  exec();
}


WorkerEventListener::WorkerEventListener(QObject *core, QObject *parent)
  : QObject(parent)
  , m_core(core)
  , m_counter(1)
{
}


WorkerEventListener::~WorkerEventListener()
{
  qDeleteAll(m_workers);
}


bool WorkerEventListener::add(const QString &listen)
{
  Worker *worker = new Worker(this);
  m_workers.append(worker);

  return worker->start(listen);
}


void WorkerEventListener::release(quint64 id)
{
  SimpleSocket *socket = this->socket(id);
  m_sockets.remove(id);

  if (socket)
    socket->deleteLater();
}


void WorkerEventListener::customEvent(QEvent *event)
{
  SCHAT_DEBUG_STREAM(this << "customEvent()" << event->type() << QThread::currentThread())

  int type = event->type();

  // Отправка пакетов.
  if (type == ServerEvent::NewPackets) {
    NewPacketsEvent *e = static_cast<NewPacketsEvent*>(event);

    QList<quint64> sockets = e->sockets();

    for (int i = 0; i < sockets.size(); ++i) {
      SimpleSocket *socket = this->socket(sockets.at(i));
      if (!socket)
        continue;

      socket->setTimestamp(e->timestamp);
      socket->send(e->packets);

      if (!e->option)
        continue;

      if (e->option == NewPacketsEvent::KillSocketOption)
        socket->leave();
      else if (e->option == NewPacketsEvent::AuthorizeSocketOption)
        socket->setAuthorized(e->userId());
    }
  }
}
