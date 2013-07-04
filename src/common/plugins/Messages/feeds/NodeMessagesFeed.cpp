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
  if (path == MESSAGES_FEED_FETCH_KEY)
    return fetch(json, channel);
  else if (path == MESSAGES_FEED_LAST_KEY)
    return last(json, channel);
  else if (path == MESSAGES_FEED_OFFLINE_KEY)
    return offline(channel);
  else if (path == MESSAGES_FEED_SINCE_KEY)
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

  QList<ChatId> ids;

  if (json.contains(MESSAGES_FEED_MESSAGES_KEY)) {
    ChatId messageId;
    const QStringList messages = json.value(MESSAGES_FEED_MESSAGES_KEY).toStringList();

    foreach (const QString &id, messages) {
      messageId.init(id.toLatin1());
      if (messageId.type() == ChatId::MessageId)
        ids.append(messageId);
    }
  }

  if (ids.isEmpty())
    return Notice::BadRequest;

  const QList<MessageRecordV2> records = NodeMessagesDB::get(ids, head().channel()->type() == ChatId::UserId ? user->id() : ChatId());
  if (records.isEmpty())
    return FeedReply(Notice::NotFound);

  FeedReply reply(Notice::OK);
  toPackets(reply.packets, records);

  reply.json[MESSAGES_FEED_COUNT_KEY] = reply.packets.size();
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
  const int count = json.value(MESSAGES_FEED_COUNT_KEY, 20).toInt();
  if (count <= 0)
    return Notice::BadRequest;

  qint64 before = json.value(MESSAGES_FEED_BEFORE_KEY, 0).toLongLong();
  QStringList messages;
  Channel *channel = head().channel();

  if (channel->type() == ChatId::ChannelId) {
    messages = MessageNotice::encode(NodeMessagesDB::last(channel->id(), count, before));
  }
  else if (channel->type() == ChatId::UserId) {
    if (!user)
      return Notice::BadRequest;

    messages = MessageNotice::encode(NodeMessagesDB::last(channel->id(), user->id(), count, before));
  }

  if (messages.isEmpty())
    return Notice::NotFound;

  if (json.contains(MESSAGES_FEED_TAG_KEY) && json.value(MESSAGES_FEED_TAG_KEY) == MessageNotice::toTag(messages))
    return Notice::NotModified;

  FeedReply reply(Notice::OK);
  reply.json = json;
  reply.json[MESSAGES_FEED_COUNT_KEY]    = messages.size();
  reply.json[MESSAGES_FEED_MESSAGES_KEY] = messages;
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

  QList<MessageRecordV2> records = NodeMessagesDB::offline(user->id());
  if (records.isEmpty())
    return Notice::NotFound;

  NodeMessagesDB::markAsRead(records);

  FeedReply reply(Notice::OK);
  toPackets(reply.packets, records);

  reply.json[MESSAGES_FEED_COUNT_KEY] = reply.packets.size();
  return reply;
}


FeedReply NodeMessagesFeed::since(const QVariantMap &json, Channel *user) const
{
  const qint64 date = json.value(MESSAGES_FEED_DATE_KEY, 0).toLongLong();
  if (date <= 0)
    return Notice::BadRequest;

  qint64 end = json.value(MESSAGES_FEED_END_KEY, DateTime::utc()).toLongLong();
  QList<ChatId> messages;
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
  reply.json[MESSAGES_FEED_COUNT_KEY]    = messages.size();
  reply.json[MESSAGES_FEED_MESSAGES_KEY] = MessageNotice::encode(messages);

  return reply;
}


void NodeMessagesFeed::toPackets(QList<QByteArray> &out, const QList<MessageRecordV2> &records) const
{
  for (int i = 0; i < records.size(); ++i) {
    const MessageRecordV2& record = records.at(i);
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
