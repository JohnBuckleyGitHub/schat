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

#include "ChatNotify.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientFeeds.h"
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
  if (json.isEmpty())
    return;

  if (!json.contains(LS("feeds")))
    return;

  QVariantMap data = json.value(LS("feeds")).toMap();
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


QStringList ClientFeedsImpl::unsynced(ClientChannel channel, const QVariantMap &feeds)
{
  QStringList extra;
  extra += LS("account");
  return Feeds::unsynced(channel, feeds, extra);
}


/*!
 * Обработка получения тела фида.
 */
void ClientFeedsImpl::feed()
{
  QString name = m_packet->text();
  if (name.isEmpty())
    return;

  FeedPtr feed = FeedPtr(FeedStorage::load(name, m_packet->json().value(name).toMap()));
  if (!feed)
    return;

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
  QVariantMap feeds = m_packet->json().value(LS("feeds")).toMap();
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

  ChatNotify::start(FeedNotify(Notify::FeedReply, m_channel->id(), name, m_packet->json()));
}
