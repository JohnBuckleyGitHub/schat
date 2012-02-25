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

  else if (action == LS("last"))
    return last(json, channel);

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

  QVariantList data = NodeMessagesDB::get(ids);
  if (data.isEmpty())
    return FeedQueryReply(Notice::NotFound);

  FeedQueryReply reply = FeedQueryReply(Notice::OK);
  toPackets(reply.packets, data);
  if (reply.packets.isEmpty())
    return FeedQueryReply(Notice::InternalError);

  reply.single = true;
  reply.json[LS("action")] = LS("get");
  reply.json[LS("count")]  = reply.packets.size();
  return reply;
}


/*!
 * Обработка запроса \b last, для получения идентификаторов последних сообщений.
 *
 * Дополнительные поля запроса:
 * - \b count - Число сообщений, обязательно.
 * - \b id    - Идентификатор пользователя, с которым идёт разговор.
 */
FeedQueryReply NodeHistoryFeed::last(const QVariantMap &json, Channel *channel)
{
  if (!channel)
    return FeedQueryReply(Notice::BadRequest);

  QList<MessageId> ids = last(json);
  if (ids.isEmpty())
    return FeedQueryReply(Notice::NotFound);

  FeedQueryReply reply = FeedQueryReply(Notice::OK);
  reply.single = true;
  reply.json[LS("action")] = LS("last");
  reply.json[LS("ids")]    = MessageId::toString(ids);
  reply.json[LS("count")]  = ids.size();

  if (json.contains(LS("id")))
    reply.json[LS("id")]  = json.value(LS("id"));

  return reply;
}


FeedQueryReply NodeHistoryFeed::offline(const QVariantMap & /*json*/, Channel *channel)
{
  if (!channel)
    return FeedQueryReply(Notice::BadRequest);

  if (head().channel()->id() != channel->id())
    return FeedQueryReply(Notice::Forbidden);

  QVariantList data = NodeMessagesDB::offline(channel->id());
  if (data.isEmpty())
    return FeedQueryReply(Notice::NotFound);

  QList<QByteArray> packets = toPackets(data);
  if (packets.isEmpty())
    return FeedQueryReply(Notice::InternalError);

  NodeMessagesDB::markAsRead(data);
  Core::send(packets);

  FeedQueryReply reply = FeedQueryReply(Notice::OK);
  reply.single = true;
  reply.json[LS("action")] = LS("offline");
  reply.json[LS("count")]  = packets.size();
  return reply;
}


/*!
 * Получение идентификаторов последних сообщений.
 */
QList<MessageId> NodeHistoryFeed::last(const QVariantMap &json)
{
  int count = json.value(LS("count")).toInt();
  if (count <= 0)
    return QList<MessageId>();

  if (head().channel()->type() == SimpleID::ChannelId)
    return NodeMessagesDB::last(head().channel()->id(), count);

  if (head().channel()->type() != SimpleID::UserId)
    return QList<MessageId>();

  QByteArray id = SimpleID::decode(json.value(LS("id")).toByteArray());
  if (SimpleID::typeOf(id) != SimpleID::UserId)
    return NodeMessagesDB::last(head().channel()->id(), count);
  else
    return NodeMessagesDB::last(head().channel()->id(), id, count);
}


QList<QByteArray> NodeHistoryFeed::toPackets(const QVariantList &data)
{
  QList<QByteArray> out;
  toPackets(out, data);
  return out;
}


void NodeHistoryFeed::toPackets(QList<QByteArray> &out, const QVariantList &data)
{
  for (int i = 0; i < data.size(); ++i) {
    QVariantList msg = data.at(i).toList();
    if (msg.isEmpty())
      continue;

    MessageNotice packet(msg.at(1).toByteArray(), msg.at(2).toByteArray(), msg.at(6).toString(), msg.at(4).toLongLong(), msg.at(0).toByteArray());
    packet.setStatus(msg.at(3).toInt());
    packet.setCommand(msg.at(5).toString());
    out.append(packet.data(Core::stream()));
  }
}
