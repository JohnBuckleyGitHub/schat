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
#include "feeds/FeedsCore.h"
#include "net/NodePool.h"
#include "NodeInit.h"
#include "NodePlugins.h"
#include "Path.h"
#include "Settings.h"
#include "sglobal.h"
#include "Storage.h"
#include "version.h"

/*!
 * Инициализация сервера.
 */
NodeInit::NodeInit(const QStringList &args, const QString &app, QObject *parent)
  : QObject(parent)
  , m_core(0)
  , m_pool(0)
  , m_args(args)
{
  new FeedsCore(this);

  m_storage = new Storage(app, this);
  m_core = new Core(this);
  m_plugins = new NodePlugins(this);

  pid();

  QTimer::singleShot(0, this, SLOT(start()));
}

NodeInit::~NodeInit()
{
  const QString pid = pidFile();
  if (!pid.isEmpty())
    QFile::remove(pid);
}

bool NodeInit::version(const QStringList &args)
{
  if (args.contains(LS("--version")) || args.contains(LS("-version"))) {
    NodeInit::version();
    return true;
  }

  return false;
}


QString NodeInit::base(const QStringList &args)
{
  return value(QStringList() << LS("--base") << LS("-base") << LS("-B"), args).toString();
}


QVariant NodeInit::value(const QString &key, const QStringList &args)
{
  if (key.isEmpty())
    return QVariant();

  int index = args.indexOf(key);
  if (index + 1 == args.size())
    return QVariant();

  if (index == -1) {
    if (key.size() > 2) {
      index = args.indexOf(QRegExp(key + LS("=*"), Qt::CaseSensitive, QRegExp::Wildcard));
      if (index != -1)
        return args.at(index).mid(key.size() + 1);
    }

    return QVariant();
  }

  return args.at(index + 1);
}


QVariant NodeInit::value(const QStringList &keys, const QStringList &args)
{
  if (keys.isEmpty())
    return QVariant();

  QVariant out;
  foreach (const QString &key, keys) {
    out = value(key, args);
    if (!out.isNull())
      return out;
  }

  return out;
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

  QStringList listen = Storage::settings()->value(STORAGE_LISTEN).toStringList();
  int workers = Storage::settings()->value(STORAGE_WORKERS).toInt();
  m_pool = new NodePool(listen, workers, m_core);
  connect(m_pool, SIGNAL(ready(QObject*)), m_core, SLOT(workerReady(QObject*)));

  m_core->start();
  m_pool->start();

  m_storage->load();
}


QString NodeInit::pidFile() const
{
  const QString file = value(QStringList() << LS("--pid") << LS("-P"), m_args).toString();

# if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
  if (file.isEmpty())
    return LS("/var/run/") + Path::app() + LS(".pid");
# endif

  return file;
}


void NodeInit::pid()
{
  const QString file = pidFile();
  if (!file.isEmpty()) {
    QFile pid(file);
    if (pid.open(QIODevice::WriteOnly))
      pid.write(QByteArray::number(QCoreApplication::applicationPid()));
  }
}
