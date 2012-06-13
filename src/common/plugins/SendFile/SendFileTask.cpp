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

#include <QBasicTimer>
#include <QFile>
#include <QTimerEvent>

#include "DateTime.h"
#include "debugstream.h"
#include "net/SimpleID.h"
#include "SendFileSocket.h"
#include "SendFileTask.h"
#include "SendFileTransaction.h"
#include "SendFileWorker.h"

namespace SendFile {

Task::Task(Worker *worker, const QVariantMap &data)
  : QObject(worker)
  , m_file(0)
  , m_pos(0)
  , m_time(0)
  , m_socket(0)
  , m_worker(worker)
{
  SCHAT_DEBUG_STREAM("[SendFile] Task::Task(), id:" << SimpleID::encode(data.value("id").toByteArray()) << this);

  m_timer = new QBasicTimer();
  m_transaction = new Transaction(data);
  m_file = new QFile(m_transaction->file().name, this);
}


Task::~Task()
{
  SCHAT_DEBUG_STREAM("[SendFile] Task::~Task()" << this);

  if (m_timer->isActive())
    m_timer->stop();

  delete m_timer;
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
  SCHAT_DEBUG_STREAM("[SendFile] Task::setSocket(), socket:" << socket << "exists:" << m_socket << this);

  if (m_socket && m_socket->mode() != Socket::DataMode)
    m_socket->leave(true);

  m_socket = socket;
  start();

  foreach (Socket *socket, m_discovery) {
    socket->leave(true);
  }
}


void Task::timerEvent(QTimerEvent *event)
{
  if (event->timerId() != m_timer->timerId()) {
    QObject::timerEvent(event);
    return;
  }

  qint64 total = m_transaction->file().size;
  emit progress(m_transaction->id(), m_pos, total, m_pos * 100 / total);
}


void Task::accepted()
{
  Socket *socket = qobject_cast<Socket*>(sender());
  if (!socket)
    return;

  SCHAT_DEBUG_STREAM("[SendFile] Task::accepted(), socket:" << socket << this);

  if (!m_socket) {
    m_socket = socket;
    start();
    m_discovery.removeAll(socket);
  }

  foreach (Socket *socket, m_discovery) {
    socket->leave(true);
  }
}


void Task::finished()
{
  m_timer->stop();
  emit finished(m_transaction->id(), DateTime::utc() - m_time);
}


void Task::progress(qint64 current)
{
  m_pos = current;
}


/*!
 * Попытка подключения к удалённой стороне.
 *
 * \param host Адрес удалённой стороны.
 * \param port Порт удалённой стороны.
 */
void Task::discovery(const QString &host, quint16 port)
{
  SCHAT_DEBUG_STREAM("[SendFile] Task::discovery(), host:" << host << ", port:" << port << this);

  Socket *socket = new Socket(host, port, m_transaction->id(), this);
  connect(socket, SIGNAL(accepted()), SLOT(accepted()));
  m_discovery.append(socket);
}


void Task::start()
{
  SCHAT_DEBUG_STREAM("[SendFile] Task::start(), socket:" << m_socket << this);

  connect(m_socket, SIGNAL(progress(qint64, qint64)), SLOT(progress(qint64)));
  connect(m_socket, SIGNAL(finished()), SLOT(finished()));
  m_socket->setFile(m_transaction->role(), m_file, m_transaction->file().size);

  m_time = DateTime::utc();
  m_timer->start(200, this);
  emit started(m_transaction->id(), m_time);
}

} // namespace SendFile
