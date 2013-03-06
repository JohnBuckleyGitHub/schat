/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include "cores/Core.h"
#include "DateTime.h"
#include "feeds/MessagesFeed.h"
#include "feeds/NodeMessagesFeed.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "NodeMessagesDB.h"
#include "ServerChannel.h"
#include "sglobal.h"

NodeMessagesFeed::NodeMessagesFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  m_header.acl().setMask(0644);
}


NodeMessagesFeed::NodeMessagesFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  m_header.acl().setMask(0644);
}


FeedReply NodeMessagesFeed::get(const QString &path, const QVariantMap &json, Channel *channel) const
{
  if (path == LS("fetch"))
    return fetch(json, channel);
  else if (path == LS("last"))
    return last(json, channel);
  else if (path == LS("offline"))
    return offline(channel);
  else if (path == LS("since"))
    return since(json, channel);
  else if (path == MESSAGES_FEED_LOGGING_KEY)
    return logging();

  return Notice::NotImplemented;
}


/*!
 * Загрузка сообщений по идентификаторам.
 */
FeedReply NodeMessagesFeed::fetch(const QVariantMap &json, Channel *user) const
{
  if (!user)
    return Notice::BadRequest;

  QList<QByteArray> ids = MessageNotice::decode(json.value(LS("messages")).toStringList());
  if (ids.isEmpty())
    return Notice::BadRequest;

  QList<MessageRecord> records = NodeMessagesDB::get(ids, head().channel()->type() == SimpleID::UserId ? user->id() : QByteArray());
  if (records.isEmpty())
    return FeedReply(Notice::NotFound);

  FeedReply reply(Notice::OK);
  toPackets(reply.packets, records);

  reply.json[LS("count")] = reply.packets.size();
  return reply;
}


/*!
 * Запрос последних сообщений в канале.
 *
 * \param json JSON данные запроса, могут содержать значение "count" для определения количества сообщений, по умолчанию 20.
 * \param user Пользователь совершивший запрос.
 */
FeedReply NodeMessagesFeed::last(const QVariantMap &json, Channel *user) const
{
  int count = json.value(LS("count"), 20).toInt();
  if (count <= 0)
    return Notice::BadRequest;

  qint64 before = json.value(LS("before"), 0).toLongLong();
  QStringList messages;
  Channel *channel = head().channel();

  if (channel->type() == SimpleID::ChannelId) {
    messages = MessageNotice::encode(NodeMessagesDB::last(channel->id(), count, before));
  }
  else if (channel->type() == SimpleID::UserId) {
    if (!user)
      return Notice::BadRequest;

    messages = MessageNotice::encode(NodeMessagesDB::last(channel->id(), user->id(), count, before));
  }

  if (messages.isEmpty())
    return Notice::NotFound;

  if (json.contains(LS("tag")) && json.value(LS("tag")) == MessageNotice::toTag(messages))
    return Notice::NotModified;

  FeedReply reply(Notice::OK);
  reply.json = json;
  reply.json[LS("count")]    = messages.size();
  reply.json[LS("messages")] = messages;
  return reply;
}


/*!
 * Возвращает Notice::OK если журналирование разрешено или Notice::Forbidden если запрещено.
 *
 * Журналирование всегда разрешено для каналов пользователей.
 */
FeedReply NodeMessagesFeed::logging() const
{
  Channel *channel = head().channel();
  if (channel->type() != SimpleID::ChannelId)
    return Notice::OK;

  FeedPtr feed = channel->feed(FEED_NAME_INFO, false, false);
  if (feed && !feed->data().value(MESSAGES_FEED_LOGGING_KEY, true).toBool())
    return Notice::Forbidden;

  return Notice::OK;
}


FeedReply NodeMessagesFeed::offline(Channel *user) const
{
  if (!user)
    return Notice::BadRequest;

  if (head().channel()->id() != user->id())
    return FeedReply(Notice::Forbidden);

  QList<MessageRecord> records = NodeMessagesDB::offline(user->id());
  if (records.isEmpty())
    return Notice::NotFound;

  NodeMessagesDB::markAsRead(records);

  FeedReply reply(Notice::OK);
  toPackets(reply.packets, records);

  reply.json[LS("count")] = reply.packets.size();
  return reply;
}


FeedReply NodeMessagesFeed::since(const QVariantMap &json, Channel *user) const
{
  qint64 date = json.value(LS("date"), 0).toLongLong();
  if (date <= 0)
    return Notice::BadRequest;

  qint64 end = json.value(LS("end"), DateTime::utc()).toLongLong();
  QList<QByteArray> messages;
  Channel *channel = head().channel();

  if (channel->type() == SimpleID::ChannelId) {
    messages = NodeMessagesDB::since(channel->id(), date, end);
  }
  else if (channel->type() == SimpleID::UserId) {
    if (!user)
      return Notice::BadRequest;

    messages = NodeMessagesDB::since(channel->id(), user->id(), date, end);
  }

  if (messages.isEmpty())
    return Notice::NotFound;

  FeedReply reply(Notice::OK);
  reply.json = json;
  reply.json[LS("count")]    = messages.size();
  reply.json[LS("messages")] = MessageNotice::encode(messages);

  return reply;
}


void NodeMessagesFeed::toPackets(QList<QByteArray> &out, const QList<MessageRecord> &records) const
{
  for (int i = 0; i < records.size(); ++i) {
    const MessageRecord& record = records.at(i);
    if (!record.id)
      continue;

    MessageNotice packet(record);
    out.append(packet.data(Core::stream()));
  }
}


Feed* NodeMessagesFeedCreator::create(const QString &name) const
{
  return new NodeMessagesFeed(name, DateTime::utc());
}


Feed* NodeMessagesFeedCreator::load(const QString &name, const QVariantMap &data) const
{
  return new NodeMessagesFeed(name, data);
}
