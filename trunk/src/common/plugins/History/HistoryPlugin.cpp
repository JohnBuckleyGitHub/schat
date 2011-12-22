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

#include <QDebug>

#include <QTime>
#include <QtPlugin>

#include "ChatCore.h"
#include "client/ChatClient.h"
#include "HistoryDB.h"
#include "HistoryPlugin.h"
#include "HistoryPlugin_p.h"
#include "NetworkManager.h"
#include "plugins/hooks.h"
#include "ui/tabs/PrivateTab.h"

History::History(ChatCore *core)
  : AbstractHistory(core)
{
  m_db = new HistoryDB(this);
  openDb();
}


QList<HookData::Type> History::hooks() const
{
  QList<HookData::Type> out;
  out += HookData::RawUserMessage;
  out += HookData::PrivateTabCreated;

  return out;
}


void History::hook(const HookData &data)
{
  switch (data.type()) {
    case HookData::RawUserMessage:
      add(static_cast<const RawUserMessageHook &>(data));
      break;

    default:
      break;
  }
}


void History::add(const RawUserMessageHook &data)
{
  QTime t;
  t.start();
  m_db->add(data.status, data.data);
  qDebug() << "[History Plugin] message added at:" << t.elapsed() << "ms";
}


void History::openDb()
{
  QByteArray id = ChatClient::serverId();
  if (!id.isEmpty())
    m_db->open(id, m_core->networks()->root(id));
}


ChatPlugin *HistoryPlugin::init(ChatCore *core)
{
  m_plugin = new History(core);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(History, HistoryPlugin);
