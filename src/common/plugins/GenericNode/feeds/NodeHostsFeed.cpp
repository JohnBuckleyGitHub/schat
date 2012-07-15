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

#include "Ch.h"
#include "Channel.h"
#include "cores/Core.h"
#include "DateTime.h"
#include "events.h"
#include "feeds/NodeHostsFeed.h"
#include "net/packets/ChannelNotice.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "ServerChannel.h"
#include "sglobal.h"
#include "tools/Ver.h"

NodeHostsFeed::NodeHostsFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  m_header.acl().setMask(0400);
}


NodeHostsFeed::NodeHostsFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  m_header.acl().setMask(0400);
}


Feed* NodeHostsFeed::create(const QString &name)
{
  return new NodeHostsFeed(name, DateTime::utc());
}


Feed* NodeHostsFeed::load(const QString &name, const QVariantMap &data)
{
  return new NodeHostsFeed(name, data);
}


/*!
 * Обработка запросов.
 */
FeedQueryReply NodeHostsFeed::query(const QVariantMap &json, Channel *channel)
{
  QString action = json.value(LS("action")).toString();
  if (action.isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  if (action.startsWith(LS("x-")))
    return Feed::query(json, channel);

  if (action == LS("activity"))
    return activity(channel);
  else if (action == LS("unlink"))
    return unlink(json, channel);

  return FeedQueryReply(Notice::ServiceUnavailable);
}


/*!
 * Получение тела фида.
 */
QVariantMap NodeHostsFeed::feed(Channel *channel)
{
  QVariantMap header = head().get(channel);
  if (header.isEmpty())
    return QVariantMap();

  QVariantMap out;
  out[LS("head")] = header;

  ServerChannel *ch = static_cast<ServerChannel *>(head().channel());
  const QHash<QByteArray, HostInfo> &hosts = ch->hosts().all();
  foreach (const HostInfo &info, hosts) {
    QVariantMap data;
    data[LS("online")]  = info->online;
    data[LS("name")]    = info->name;
    data[LS("host")]    = info->address;
    data[LS("version")] = Ver(info->version).toString();
    data[LS("os")]      = info->os;
    data[LS("osName")]  = info->osName;
    data[LS("tz")]      = info->tz;
    data[LS("date")]    = info->date;

    merge(LS("geo"),  data, info->geo);
    merge(LS("data"), data, info->data);
    out[SimpleID::encode(info->hostId)] = data;
  }

  return out;
}


/*!
 * Обработка запроса \b activity.
 */
FeedQueryReply NodeHostsFeed::activity(Channel *channel)
{
  if (!channel || head().channel()->id() != channel->id())
    return FeedQueryReply(Notice::Forbidden);

  qint64 current = DateTime::utc();
  FeedQueryReply reply = FeedQueryReply(Notice::OK);

  QMapIterator<QString, QVariant> i(m_data);
  while (i.hasNext()) {
    i.next();
    if (i.key().size() != 34)
      continue;

    qint64 date = i.value().toMap().value(LS("date")).toLongLong();
    if (date == 0)
      continue;

    QVariantMap data;
    data[LS("date")] = date;
    data[LS("diff")] = current - date;
    reply.json[i.key()] = data;
  }

  reply.json[LS("action")] = LS("activity");
  return reply;
}


/*!
 * Обработка запроса \b unlink.
 */
FeedQueryReply NodeHostsFeed::unlink(const QVariantMap &json, Channel *channel)
{
  if (!channel || head().channel()->id() != channel->id())
    return FeedQueryReply(Notice::Forbidden);

  QString id = json.value(LS("id")).toString();
  if (id.size() != 34) {
    ChatChannel user = Ch::channel(head().channel()->id());
    if (!channel)
      return FeedQueryReply(Notice::BadRequest);

    id = SimpleID::encode(user->hosts().currentId());
  }

  if (!m_data.contains(id))
    return FeedQueryReply(Notice::NotFound);

  QVariantMap data = m_data.value(id).toMap();
  m_data.remove(id);

  kick(SimpleID::decode(id), data);

  FeedQueryReply reply = FeedQueryReply(Notice::OK);
  reply.json[LS("action")] = LS("unlink");
  reply.modified = true;
  return reply;
}


void NodeHostsFeed::kick(const QByteArray &id, const QVariantMap &data)
{
  if (data.value(LS("online")) == false)
    return;

  ChatChannel channel = Ch::channel(head().channel()->id());
  if (!channel)
    return;

  QList<quint64> sockets = channel->hosts().sockets(id);
  if (sockets.isEmpty())
    return;

  Core::i()->send(sockets, ChannelNotice::request(channel->id(), channel->id(), LS("quit"))->data(Core::stream()), NewPacketsEvent::KillSocketOption);
}
