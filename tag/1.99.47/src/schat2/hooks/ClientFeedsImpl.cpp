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
#include "feeds/FeedStorage.h"
#include "hooks/ClientFeedsImpl.h"
#include "net/packets/FeedNotice.h"
#include "net/SimpleID.h"
#include "sglobal.h"

ClientFeedsImpl::ClientFeedsImpl(QObject *parent)
  : Feeds(parent)
{
  ChatClient::feeds()->hooks()->add(this);
}


void ClientFeedsImpl::addImpl(ClientChannel channel, const ChannelInfo & /*info*/, const QVariantMap &json)
{
  SCHAT_DEBUG_STREAM("ClientFeedsImpl::addImpl()" << channel->name() << json.keys())

  if (json.isEmpty() || !json.contains(LS("f")))
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

  const QString& cmd = packet.command();
  if (cmd.isEmpty())
    return;

  m_packet = &packet;

  if (cmd == LS("feed"))
    feed();
  else if (cmd == LS("get"))
    get();
  else if (cmd == LS("put") || cmd == LS("post") || cmd == LS("delete"))
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

  QVariantMap data;
  FeedPtr feed = m_channel->feed(name, false, false);
  if (feed)
    data = feed->head().data();

  feed = FeedPtr(FeedStorage::load(name, m_packet->json().value(name).toMap()));
  if (!feed)
    return;

  FeedHeader &head = feed->head();
  head.setData(data);
  head.setDate(m_packet->date());

  m_channel->feeds().add(feed);
  ChatNotify::start(new FeedNotify(Notify::FeedData, m_channel->id(), name));

  if (data.size() > 2 && data.value(LS("date")) != head.date())
    ClientFeeds::request(m_channel, LS("get"), name + LS("/head"));
}


/*!
 * Обработка получения ответа на get запрос.
 */
void ClientFeedsImpl::get()
{
  QPair<QString, QString> request = FeedNotice::split(m_packet->text());
  if (request.first == LS("*")) {
    headers();
  }
  else if (request.second.isEmpty()) {
    if (m_packet->status() == Notice::NotModified && m_channel->feed(request.first, false))
      ChatNotify::start(new FeedNotify(Notify::FeedData, m_channel->id(), request.first));
  }
  else {
    if (request.second == LS("head")) {
      FeedPtr feed = m_channel->feed(request.first, false, false);
      if (!feed)
        feed = FeedPtr(FeedStorage::load(request.first, QVariantMap()));

      if (feed) {
        feed->head().setData(m_packet->json());
        m_channel->feeds().add(feed);
      }
    }

    ChatNotify::start(new FeedNotify(m_channel->id(), m_packet));
  }
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

  foreach (const QString &name, feeds) {
    ClientFeeds::request(id, LS("get"), name);
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


void ClientFeedsImpl::reply()
{
  if (m_packet->text().isEmpty())
    return;

  FeedNotify *notify = new FeedNotify(m_channel->id(), m_packet);
  ChatNotify::start(notify);

  if (m_packet->status() != 200 || notify->path().startsWith("head/"))
    return;

  FeedPtr feed = ChatClient::channel()->feed(notify->feed(), false);
  if (!feed)
    return;

  if (notify->command() == LS("delete")) {
    if (notify->path().isEmpty()) {
      FeedStorage::remove(feed);
      m_channel->feeds().remove(notify->feed());
      return;
    }

    if (notify->path() == LS("*"))
      feed->data().clear();
    else
      feed->data().remove(notify->path());
  }
  else {
    if (!m_packet->json().contains(LS("value")))
      return;

    feed->data()[notify->path()] = m_packet->json().value(LS("value"));
  }

  feed->head().setDate(m_packet->date());
  FeedStorage::save(feed);
  ChatNotify::start(new FeedNotify(Notify::FeedData, m_channel->id(), notify->feed()));
}
