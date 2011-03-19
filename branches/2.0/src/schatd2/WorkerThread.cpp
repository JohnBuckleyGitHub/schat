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

#include "Core.h"
#include "debugstream.h"
#include "Worker.h"
#include "WorkerThread.h"

WorkerThread::WorkerThread(Core *parent)
  : QThread(0),
    m_core(parent)
{
}


WorkerThread::~WorkerThread()
{
  SCHAT_DEBUG_STREAM("~" << this)

  qDeleteAll(m_workers);
}


void WorkerThread::run()
{
  int workerId = 0;
  m_workers.append(new Worker(workerId, m_core));
  m_workers.at(workerId)->start();

  emit workersStarted();

  exec();
}
