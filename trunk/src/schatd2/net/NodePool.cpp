/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include <QDebug>

#include <QCoreApplication>

#include "events.h"
#include "net/NodePool.h"
#include "net/NodeWorker.h"
#include "net/TcpServer.h"

NodePool::NodePool(const QStringList &listen, QObject *core, QObject *parent)
  : QThread(parent)
  , m_core(core)
  , m_listen(listen)
  , m_counter(0)
{
  qDebug() << " ~~~ NodePool" << currentThread() << QThread::idealThreadCount();
}


NodePool::~NodePool()
{
  qDebug() << " ------------------------- NodePool::~NodePool()";

  foreach (TcpServer *server, m_servers) {
    server->close();
    delete server;
  }

  foreach (NodeWorker *worker, m_workers) {
    worker->quit();
  }

  foreach (NodeWorker *worker, m_workers) {
    worker->wait();
    delete worker;
  }
}


void NodePool::run()
{
  qDebug() << " ~~~ NodePool::run()" << currentThread();

  for (int i = 0; i < 2; ++i) {
    NodeWorker *worker = new NodeWorker(m_core);
    m_workers.append(worker);
    connect(worker, SIGNAL(ready(NodeWorkerListener *)), SLOT(workerReady(NodeWorkerListener *)));

    worker->start();
  }

  foreach (QString host, m_listen) {
    TcpServer *server = new TcpServer;
    m_servers.append(server);
    connect(server, SIGNAL(newConnection(int)), SLOT(newConnection(int)), Qt::DirectConnection);
    server->listen(host);
  }

  exec();
}


/*!
 * Обработка нового подключения.
 */
void NodePool::newConnection(int socketDescriptor)
{
  qDebug() << " ~~~ NodePool::newConnection()" << currentThread();

  m_counter++;
  QCoreApplication::postEvent(m_listeners.at(0), new NewConnectionEvent(socketDescriptor, m_counter));
}


/*!
 * Обработка готовности потока NodeWorker.
 */
void NodePool::workerReady(NodeWorkerListener *listener)
{
  qDebug() << " ~~~ NodePool::workerReady()" << currentThread() << listener;
  m_listeners.append(listener);
  qDebug() << m_listeners.size();
  emit ready(listener);
}
