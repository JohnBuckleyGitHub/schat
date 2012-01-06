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

#include <QTime>
#include <QtPlugin>

#include "ChatCore.h"
#include "client/ChatClient.h"
#include "HistoryDB.h"
#include "HistoryPlugin.h"
#include "HistoryPlugin_p.h"
#include "NetworkManager.h"
#include "ui/tabs/PrivateTab.h"

History::History(QObject *parent)
  : ChatPlugin(parent)
{
  m_db = new HistoryDB(this);
  openDb();
}


void History::openDb()
{
  QByteArray id = ChatClient::serverId();
  if (!id.isEmpty())
    m_db->open(id, ChatCore::networks()->root(id));
}


ChatPlugin *HistoryPlugin::create()
{
  m_plugin = new History(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(History, HistoryPlugin);
