/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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
#include <QTextStream>
#include <QTimer>

#include "cores/Core.h"
#include "debugstream.h"
#include "net/NodePool.h"
#include "NodeInit.h"
#include "NodeNotify.h"
#include "NodePlugins.h"
#include "Path.h"
#include "Settings.h"
#include "sglobal.h"
#include "Storage.h"
#include "version.h"

/*!
 * Инициализация сервера.
 */
NodeInit::NodeInit(const QString &app, QObject *parent)
  : QObject(parent)
  , m_core(0)
  , m_pool(0)
{
  new NodeNotify(this);

  m_storage = new Storage(app, this);
  m_core = new Core(this);
  m_plugins = new NodePlugins(this);

# if defined(SCHATD_SLAVE)
  m_plugins->setType("slave");
# endif

  QTimer::singleShot(0, this, SLOT(start()));

# if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
  QFile pid(LS("/var/run/") + Path::app() + LC('/') + Path::app() + LS(".pid"));
  if (pid.open(QIODevice::WriteOnly))
    pid.write(QByteArray::number(QCoreApplication::applicationPid()));
# endif
}


bool NodeInit::version(const QStringList &arguments)
{
  if (arguments.contains(LS("--version")) || arguments.contains(LS("-version"))) {
    NodeInit::version();
    return true;
  }

  return false;
}


QString NodeInit::base(const QStringList &arguments)
{
  int index = arguments.indexOf(LS("-base"));
  if (index == -1 || index + 1 == arguments.size())
    return QString();

  return arguments.at(index + 1);
}


void NodeInit::version()
{
  QTextStream out(stdout);
  out << LS("Simple Chat Daemon, version ") << SCHAT_VERSION;
  if (SCHAT_REVISION)
    out << LC('.') << SCHAT_REVISION;

  out << endl << endl;
  out << QString(SCHAT_COPYRIGHT).replace("©", LS("(C)")) << LS(", https://schat.me") << endl;
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
