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

  if (action == LS("unlink"))
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

  ServerChannel *user = static_cast<ServerChannel *>(head().channel());
  const QHash<QByteArray, HostInfo> &hosts = user->hosts()->all();
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
 * Обработка запроса \b unlink.
 */
FeedQueryReply NodeHostsFeed::unlink(const QVariantMap &json, Channel *channel)
{
  if (!channel || head().channel()->id() != channel->id())
    return FeedQueryReply(Notice::Forbidden);

  QByteArray id = SimpleID::decode(json.value(LS("id")).toString());
  if (SimpleID::typeOf(id) != SimpleID::HostId)
    return FeedQueryReply(Notice::BadRequest);

  ServerChannel *user = static_cast<ServerChannel *>(head().channel());
  if (!user->hosts()->all().contains(id))
    return FeedQueryReply(Notice::NotFound);

  user->hosts()->unlink(id);

  FeedQueryReply reply = FeedQueryReply(Notice::OK);
  reply.json[LS("action")] = LS("unlink");
  reply.modified = true;
  return reply;
}
