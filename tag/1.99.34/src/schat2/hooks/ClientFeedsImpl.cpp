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

#include "ChatNotify.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientFeeds.h"
#include "debugstream.h"
#include "feeds/AccountFeed.h"
#include "feeds/FeedStorage.h"
#include "hooks/ClientFeedsImpl.h"
#include "net/packets/FeedNotice.h"
#include "net/SimpleID.h"
#include "sglobal.h"

ClientFeedsImpl::ClientFeedsImpl(QObject *parent)
  : Feeds(parent)
{
  FeedStorage::add(new AccountFeed());
  ChatClient::feeds()->hooks()->add(this);
}


void ClientFeedsImpl::addImpl(ClientChannel channel, const ChannelInfo & /*info*/, const QVariantMap &json)
{
  SCHAT_DEBUG_STREAM("ClientFeedsImpl::addImpl()" << channel->name() << json.keys())

  if (json.isEmpty())
    return;

  if (!json.contains(LS("f")))
    return;

  QVariantMap data = json.value(LS("f")).toMap();
  if (data.isEmpty())
    return;

  get(channel->id(), unsynced(channel, data));
}


void ClientFeedsImpl::readFeedImpl(const FeedNotice &packet)
{
  m_channel = ChatClient::channels()->get(packet.sender());
  if (!m_channel)
    return;

  QString cmd = packet.command();
  if (cmd.isEmpty())
    return;

  m_packet = &packet;

  if (cmd == LS("feed"))
    feed();
  else if (cmd == LS("headers"))
    headers();
  else if (cmd == LS("query"))
    query();
  else if (cmd == LS("reply"))
    reply();
}


/*!
 * Обработка получения тела фида.
 */
void ClientFeedsImpl::feed()
{
  QString name = m_packet->text();
  if (name.isEmpty())
    return;

  qint64 key = 0;
  qint64 rev = 0;
  FeedPtr feed = m_channel->feed(name, false);
  if (feed) {
    key = feed->head().key();
    rev = feed->head().rev();
  }

  feed = FeedPtr(FeedStorage::load(name, m_packet->json().value(name).toMap()));
  if (!feed)
    return;

  if (feed->head().rev() != rev)
    key = 0;

  feed->head().setKey(key);
  m_channel->feeds().add(feed);
  ChatNotify::start(FeedNotify(Notify::FeedData, m_channel->id(), name));
}



/*!
 * Загрузка списка фидов.
 *
 * \param id    идентификатор канала.
 * \param feeds список фидов.
 */
void ClientFeedsImpl::get(const QByteArray &id, const QStringList &feeds)
{
  if (feeds.isEmpty())
    return;

  foreach (QString name, feeds) {
    ChatClient::feeds()->request(id, LS("get"), name);
  }
}


/*!
 * Обработка получения заголовков фидов.
 */
void ClientFeedsImpl::headers()
{
  QString key = LS("f");
  if (m_packet->json().contains(LS("feeds")))
    key = LS("feeds");

  QVariantMap feeds = m_packet->json().value(key).toMap();
  if (feeds.isEmpty())
    return;

  get(m_channel->id(), unsynced(m_channel, feeds));
}


void ClientFeedsImpl::query()
{
  QString name = m_packet->text();
  if (name.isEmpty())
    return;

  if (m_packet->status() == Notice::OK)
    return;

  ChatNotify::start(FeedNotify(Notify::QueryError, m_channel->id(), name, m_packet->json(), m_packet->status()));
}


void ClientFeedsImpl::reply()
{
  QString name = m_packet->text();
  if (name.isEmpty())
    return;

  FeedNotify notify(Notify::FeedReply, m_channel->id(), name, m_packet->json());
  if (notify.action() == LS("x-set"))
    set(notify);

  ChatNotify::start(notify);
}


void ClientFeedsImpl::set(const FeedNotify &notify)
{
  QStringList keys = notify.json().keys();
  keys.removeAll(LS("action"));
  keys.removeAll(LS("date"));
  keys.removeAll(LS("size"));
  if (keys.isEmpty())
    return;

  qint64 date = notify.json().value(LS("date")).toLongLong();
  if (date == 0)
    return;

  int size = notify.json().value(LS("size")).toInt();
  if (size == 0)
    return;

  FeedPtr feed = ChatClient::channel()->feed(notify.name(), false);
  if (!feed)
    return;

  feed->head().data()[LS("date")] = date;
  feed->head().data()[LS("size")] = size;

  foreach (QString key, keys) {
    feed->data()[key] = notify.json().value(key);
  }

  FeedStorage::save(feed);
}
