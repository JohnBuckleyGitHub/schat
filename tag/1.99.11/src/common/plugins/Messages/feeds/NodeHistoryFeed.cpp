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

#include "Channel.h"
#include "cores/Core.h"
#include "DateTime.h"
#include "feeds/NodeHistoryFeed.h"
#include "net/packets/MessageNotice.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "NodeMessagesDB.h"
#include "sglobal.h"

NodeHistoryFeed::NodeHistoryFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  m_header.acl().setMask(0444);
}


NodeHistoryFeed::NodeHistoryFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  m_header.acl().setMask(0444);
}


Feed* NodeHistoryFeed::create(const QString &name)
{
  return new NodeHistoryFeed(name, DateTime::utc());
}


Feed* NodeHistoryFeed::load(const QString &name, const QVariantMap &data)
{
  return new NodeHistoryFeed(name, data);
}


FeedQueryReply NodeHistoryFeed::query(const QVariantMap &json, Channel * /*channel*/)
{
  QString action = json.value(LS("action")).toString();
  if (action.isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  if (action == LS("last"))
    return last(json);

  return FeedQueryReply(Notice::NotImplemented);
}


void NodeHistoryFeed::setChannel(Channel *channel)
{
  Feed::setChannel(channel);

  if (channel->type() == SimpleID::UserId)
    head().acl().setMask(0400);
}



FeedQueryReply NodeHistoryFeed::last(const QVariantMap &json)
{
  int count = json.value(LS("count")).toInt();
  if (count <= 0)
    return FeedQueryReply(Notice::BadRequest);

  QVariantList data = NodeMessagesDB::last(head().channel()->id(), count);
  if (data.isEmpty())
    return FeedQueryReply(Notice::NotFound);

  QList<QByteArray> packets;

  for (int i = 0; i < data.size(); ++i) {
    QVariantList msg = data.at(i).toList();
    if (msg.isEmpty())
      continue;

    MessageNotice packet(msg.value(1).toByteArray(), msg.value(2).toByteArray(), msg.value(5).toString(), msg.value(4).toLongLong(), msg.value(0).toByteArray());
    int status = msg.value(3).toInt();
    if (status == Notice::OK)
      status = Notice::Found;

    packet.setStatus(status);
    packets.append(packet.data(Core::stream()));
  }

  if (packets.isEmpty())
    return FeedQueryReply(Notice::InternalError);

  Core::send(packets);

  FeedQueryReply reply     = FeedQueryReply(Notice::OK);
  reply.json[LS("action")] = LS("last");
  reply.json[LS("count")]  = packets.size();
  return reply;
}
