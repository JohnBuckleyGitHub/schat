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

#include <QFile>
#include <QBuffer>
#include <QUdpSocket>

#include "SendFileSocket.h"
#include "SendFileTask.h"
#include "SendFileTransaction.h"
#include "SendFileWorker.h"

namespace SendFile {

Task::Task(Worker *worker, const QVariantMap &data)
  : QObject(worker)
  , m_file(0)
  , m_socket(0)
  , m_worker(worker)
{
  m_transaction = new Transaction(data);
  m_file = new QFile(m_transaction->file().name, this);
}


Task::~Task()
{
  delete m_transaction;
}


bool Task::init()
{
  if (!m_file->open(m_transaction->role() ? QIODevice::WriteOnly : QIODevice::ReadOnly))
    return false;

  discovery();
  return true;
}


/*!
 * Попытка подключения к удалённой стороне.
 */
void Task::discovery()
{
  if (m_socket || !m_transaction->remote().isValid())
    return;

  QString host = m_transaction->remote().address(Internal);
  quint16 port = m_transaction->remote().port(Internal);
  discovery(host, port);

  if (m_transaction->remote().address() != host || m_transaction->remote().port() != port)
    discovery(m_transaction->remote().address(), m_transaction->remote().port());
}


void Task::setSocket(Socket *socket)
{
  qDebug() << "Task::setSocket()" << m_socket;
  if (m_socket)
    m_socket->leave(true);

  m_socket = socket;
  m_socket->setFile(m_transaction->role(), m_file);

  foreach (Socket *socket, m_discovery) {
    socket->leave(true);
  }
}


void Task::accepted()
{
  qDebug() << "Task::accepted()" << m_discovery.indexOf(qobject_cast<Socket*>(sender()));
  Socket *socket = qobject_cast<Socket*>(sender());
  if (!socket)
    return;

  if (!m_socket) {
    m_socket = socket;
    m_socket->setFile(m_transaction->role(), m_file);
    m_discovery.removeAll(socket);
  }

  foreach (Socket *socket, m_discovery) {
    socket->leave(true);
  }
}


/*!
 * Попытка подключения к удалённой стороне.
 *
 * \param host Адрес удалённой стороны.
 * \param port Порт удалённой стороны.
 */
void Task::discovery(const QString &host, quint16 port)
{
  qDebug() << "Task::discovery()" << host << port;

  Socket *socket = new Socket(host, port, m_transaction->id(), this);
  connect(socket, SIGNAL(accepted()), SLOT(accepted()));
  m_discovery.append(socket);
}

} // namespace SendFile
