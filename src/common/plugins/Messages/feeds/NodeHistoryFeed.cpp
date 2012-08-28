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


FeedQueryReply NodeHistoryFeed::query(const QVariantMap &json, Channel *channel)
{
  QString action = json.value(LS("action")).toString();
  if (action.isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  if (action == LS("get"))
    return get(json, channel);

  else if (action == LS("offline"))
    return offline(json, channel);

  return FeedQueryReply(Notice::NotImplemented);
}


void NodeHistoryFeed::setChannel(Channel *channel)
{
  Feed::setChannel(channel);

  if (channel->type() == SimpleID::UserId)
    head().acl().setMask(0400);
}


FeedQueryReply NodeHistoryFeed::get(const QVariantMap &json, Channel *channel)
{
  if (!channel)
    return FeedQueryReply(Notice::BadRequest);

  if (head().channel()->id() != channel->id())
    return FeedQueryReply(Notice::Forbidden);

  QList<MessageId> ids = MessageId::toList(json.value(LS("ids")).toString());
  if (ids.isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  QList<MessageRecord> records = NodeMessagesDB::get(ids);
  if (records.isEmpty())
    return FeedQueryReply(Notice::NotFound);

  FeedQueryReply reply = FeedQueryReply(Notice::OK);
  toPackets(reply.packets, records);
  if (reply.packets.isEmpty())
    return FeedQueryReply(Notice::InternalError);

  reply.single = true;
  reply.json[LS("action")] = LS("get");
  reply.json[LS("count")]  = reply.packets.size();
  return reply;
}


FeedQueryReply NodeHistoryFeed::offline(const QVariantMap &json, Channel *channel)
{
  Q_UNUSED(json)

  if (!channel)
    return FeedQueryReply(Notice::BadRequest);

  if (head().channel()->id() != channel->id())
    return FeedQueryReply(Notice::Forbidden);

  QList<MessageRecord> records = NodeMessagesDB::offline(channel->id());
  if (records.isEmpty())
    return FeedQueryReply(Notice::NotFound);

  QList<QByteArray> packets = toPackets(records);
  if (packets.isEmpty())
    return FeedQueryReply(Notice::InternalError);

  NodeMessagesDB::markAsRead(records);
  Core::send(packets);

  FeedQueryReply reply = FeedQueryReply(Notice::OK);
  reply.single = true;
  reply.json[LS("action")] = LS("offline");
  reply.json[LS("count")]  = packets.size();
  return reply;
}


QList<QByteArray> NodeHistoryFeed::toPackets(const QList<MessageRecord> &data)
{
  QList<QByteArray> out;
  toPackets(out, data);
  return out;
}


void NodeHistoryFeed::toPackets(QList<QByteArray> &out, const QList<MessageRecord> &records)
{
  for (int i = 0; i < records.size(); ++i) {
    const MessageRecord& record = records.at(i);
    if (!record.id)
      continue;

    MessageNotice packet(record);
    out.append(packet.data(Core::stream()));
  }
}
