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

#include "debugstream.h"
#include "net/SimpleID.h"
#include "SendFileSocket.h"
#include "SendFileTask.h"
#include "SendFileWorker.h"
#include "sglobal.h"

namespace SendFile {

Worker::Worker(quint16 port, QObject *parent)
  : QTcpServer(parent)
  , m_port(port)
{
  listen(QHostAddress::Any, m_port);

  SCHAT_DEBUG_STREAM("[SendFile] Worker::Worker(), port:" << port);
# if defined(SCHAT_DEBUG)
  if (!isListening())
    qDebug() << "        ERROR:" << errorString();
# endif
}


/*!
 * Добавление задачи.
 */
void Worker::addTask(const QVariantMap &data)
{
  SCHAT_DEBUG_STREAM("[SendFile] Worker::add(), id:" << SimpleID::encode(data.value("id").toByteArray()));

  QByteArray id = data.value("id").toByteArray();
  if (m_tasks.contains(id)) {
    updateTask(id, data);
    return;
  }

  SendFileTask task(new Task(this, data));
  if (!task->transaction()->isValid())
    return;

  if (!task->init())
    return;

  connect(task.data(), SIGNAL(finished(QByteArray, qint64)), SLOT(taskFinished(QByteArray, qint64)));
  connect(task.data(), SIGNAL(progress(QByteArray, qint64, qint64, int)), SIGNAL(progress(QByteArray, qint64, qint64, int)));
  connect(task.data(), SIGNAL(started(QByteArray, qint64)), SIGNAL(started(QByteArray, qint64)));
  m_tasks[id] = task;
}


void Worker::removeTask(const QByteArray &id)
{
  SCHAT_DEBUG_STREAM("[SendFile] Worker::removeTask(), id:" << SimpleID::encode(id));

  SendFileTask task = m_tasks.value(id);
  if (task && task->socket())
    task->socket()->leave();

  m_tasks.remove(id);
}


void Worker::accepted()
{
  Socket *socket = qobject_cast<Socket*>(sender());
  if (!socket)
    return;

  SCHAT_DEBUG_STREAM("[SendFile] Worker::accepted()" << socket->peerAddress().toString() << "socket:" << socket);
  SendFileTask task = m_tasks.value(socket->id());
  if (task)
    task->setSocket(socket);
}


/*!
 * Обработка запроса на авторизацию от входящего подключения.
 */
void Worker::handshake(const QByteArray &id)
{
  Socket *socket = qobject_cast<Socket*>(sender());
  if (!socket)
    return;

  SCHAT_DEBUG_STREAM("[SendFile] Worker::handshake()" << socket->peerAddress().toString() << "socket:" << socket);
  if (!m_tasks.contains(id))
    socket->reject();

  SendFileTask task = m_tasks.value(id);
  if (!task || task->socket())
    socket->reject();
  else
    socket->accept();
}


/*!
 * Завершение работы задачи.
 *
 * \param id      Идентификатор передачи файла.
 * \param elapsed Число миллисекунд потраченное на передачу.
 */
void Worker::taskFinished(const QByteArray &id, qint64 elapsed)
{
  SCHAT_DEBUG_STREAM("[SendFile] Worker::taskFinished(), id:" << SimpleID::encode(id) << "elapsed:" << elapsed << "ms");

  emit finished(id, elapsed);
  m_tasks.remove(id);
}


void Worker::incomingConnection(int socketDescriptor)
{
  Socket *socket = new Socket(this);
  if (!socket->setSocketDescriptor(socketDescriptor)) {
    socket->deleteLater();
    return;
  }

  SCHAT_DEBUG_STREAM("[SendFile] Worker::incomingConnection()" << "socket:" << socket);

  connect(socket, SIGNAL(accepted()), SLOT(accepted()));
  connect(socket, SIGNAL(handshake(QByteArray)), SLOT(handshake(QByteArray)));
}


void Worker::updateTask(const QByteArray &id, const QVariantMap &data)
{
  SCHAT_DEBUG_STREAM("[SendFile] Worker::updateTask()" << SimpleID::encode(id))

  SendFileTask task = m_tasks.value(id);
  if (!task)
    return;

  if (task->transaction()->role() == SenderRole && !task->transaction()->remote().isValid()) {
    Hosts hosts(data.value(LS("remote")).toList());
    if (hosts.isValid()) {
      task->transaction()->setRemote(hosts);
      task->discovery();
    }
  }
}

} // namespace SendFile
