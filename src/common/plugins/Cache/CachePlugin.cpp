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

#include <QtPlugin>
#include <QSqlDatabase>

#include "CachePlugin.h"
#include "CachePlugin_p.h"
#include "ChatCore.h"
#include "client/ChatClient.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"

Cache::Cache(QObject *parent)
  : ChatPlugin(parent)
{
  open();
}


void Cache::close()
{
  m_id.clear();
  QSqlDatabase::removeDatabase(m_id);
}


void Cache::open()
{
  QByteArray id = ChatClient::serverId();
  if (!id.isEmpty())
    open(id, ChatCore::networks()->root(id));
}


void Cache::open(const QByteArray &id, const QString &dir)
{
  if (!m_id.isEmpty())
    close();

  m_id = SimpleID::encode(id) + "-cache";

  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_id);
  db.setDatabaseName(dir + "/cache.sqlite");
  if (!db.open())
    return;
}


ChatPlugin *CachePlugin::create()
{
  m_plugin = new Cache(this);
  return m_plugin;
}


Q_EXPORT_PLUGIN2(Cache, CachePlugin);
