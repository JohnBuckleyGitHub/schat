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

#include <QTimer>

#include "debugstream.h"

#include "cores/Core.h"
#include "NodeInit.h"
#include "Settings.h"
#include "Storage.h"
#include "WorkerThread.h"

/*!
 * Инициализация сервера.
 */
NodeInit::NodeInit(QObject *parent)
  : QObject(parent)
{
  m_storage = new Storage(this);

  m_core = new Core(this); // FIXME Создание Core.
  m_core->setStorage(m_storage);

  m_thread = new WorkerThread(m_storage->settings()->value(QLatin1String("Listen")).toStringList(), m_core);
  connect(m_thread, SIGNAL(ready(QObject *)), m_core, SLOT(workersReady(QObject *)));

  QTimer::singleShot(0, this, SLOT(start()));
}


void NodeInit::quit()
{
  m_thread->quit();
  m_thread->wait();

  m_core->quit();
}


void NodeInit::start()
{
  m_storage->start();
  m_core->start();
  m_thread->start();

  SCHAT_DEBUG_STREAM("NODE STARTED");
}
