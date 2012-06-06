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

#include <QThread>
#include <QDebug>
#include <QDateTime>
#define TIMESTAMP QDateTime::currentDateTime().toString("hh:mm:ss.zzz").toLatin1().constData()

#include "SendFileWorker.h"
#include "SendFileTask.h"

namespace SendFile {

Worker::Worker(quint16 port, QObject *parent)
  : QTcpServer(parent)
{
  listen(QHostAddress::Any, port);
}


void Worker::addTask(const QVariantMap &data)
{
  qDebug() << TIMESTAMP << "Worker::add()                      " << QThread::currentThread();
  SendFileTask task(new Task(this, data));
  if (!task->transaction()->isValid())
    return;

  if (!task->init())
    return;

  qDebug() << "check ok";
}

} // namespace SendFile
