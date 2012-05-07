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

#include <QTimer>
#include <QCoreApplication>

#include "debugstream.h"

#include "cores/Core.h"
#include "NodeInit.h"
#include "NodePlugins.h"
#include "Settings.h"
#include "Storage.h"
#include "net/NodePool.h"

/*!
 * Инициализация сервера.
 */
NodeInit::NodeInit(QObject *parent)
  : QObject(parent)
  , m_core(0)
  , m_pool(0)
{
  m_storage = new Storage(this);
  m_core = new Core(this);
  m_plugins = new NodePlugins(this);

# if defined(SCHATD_SLAVE)
  m_plugins->setType("slave");
# endif

  QTimer::singleShot(0, this, SLOT(start()));
}


void NodeInit::quit()
{
  if (m_pool) {
    m_pool->quit();
    m_pool->wait();
    delete m_pool;
  }

  if (m_core)
    m_core->quit();
}


void NodeInit::start()
{
  m_storage->start();
  m_plugins->load();

  QStringList listen = Storage::settings()->value("Listen").toStringList();
  int workers = Storage::settings()->value("Workers").toInt();
  m_pool = new NodePool(listen, workers, m_core);
  connect(m_pool, SIGNAL(ready(QObject *)), m_core, SLOT(workerReady(QObject *)));

  m_core->start();
  m_pool->start();

  m_storage->load();

  SCHAT_DEBUG_STREAM("NODE STARTED");
}
