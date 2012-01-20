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

#include <QtPlugin>

#include "CacheChannels.h"
#include "CacheDB.h"
#include "CachePlugin.h"
#include "CachePlugin_p.h"
#include "ChatCore.h"
#include "client/ChatClient.h"
#include "feeds/CacheFeeds.h"
#include "feeds/CacheFeedStorage.h"
#include "NetworkManager.h"

Cache::Cache(QObject *parent)
  : ChatPlugin(parent)
{
  new Hooks::CacheChannels(this);
  new Hooks::CacheFeeds(this);
  new CacheFeedStorage(this);
  open();

  connect(ChatClient::i(), SIGNAL(online()), SLOT(open()));
}


void Cache::open()
{
  QByteArray id = ChatClient::serverId();
  if (id.isEmpty())
    return;

  if (!CacheDB::open(id, ChatCore::networks()->root(id)))
    return;

  qDebug() << " $$ " << ChatClient::serverId().toHex();
  if (ChatClient::serverId().isEmpty())
    return;

  ChatClient::server()->setId(ChatClient::serverId());
  load(ChatClient::server());
  load(ChatClient::channel());
}


void Cache::load(ClientChannel channel)
{
  qDebug() << " % " << channel->key() << channel->id().toHex();

  ClientChannel exist = CacheDB::channel(channel->id(), false);
  if (!exist) {
    channel->data().clear();
    return;
  }

  channel->setData(exist->data());
  FeedStorage::load(channel.data());
}


ChatPlugin *CachePlugin::create()
{
  m_plugin = new Cache(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(Cache, CachePlugin);
