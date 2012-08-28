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

#include "DateTime.h"
#include "feeds/NodeMessagesFeed.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "ServerChannel.h"
#include "sglobal.h"
#include "NodeMessagesDB.h"

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


Feed* NodeMessagesFeed::create(const QString &name)
{
  return new NodeMessagesFeed(name, DateTime::utc());
}


Feed* NodeMessagesFeed::load(const QString &name, const QVariantMap &data)
{
  return new NodeMessagesFeed(name, data);
}


FeedReply NodeMessagesFeed::get(const QString &path, const QVariantMap &json, Channel *channel)
{
  if (path == LS("last"))
    return last(json, channel);

  return FeedReply(Notice::NotImplemented);
}


/*!
 * Запрос последних сообщений в канале.
 *
 * \param json JSON данные запроса, могут содержать значение "count" для определения количества сообщений, по умолчанию 20.
 * \param user Пользователь совершивший запрос.
 */
FeedReply NodeMessagesFeed::last(const QVariantMap &json, Channel *user)
{
  int count = json.value(LS("count"), 20).toInt();
  if (count <= 0)
    return FeedReply(Notice::BadRequest);

  QList<QByteArray> messages;
  Channel *channel = head().channel();

  if (channel->type() == SimpleID::ChannelId) {
    messages = NodeMessagesDB::last(head().channel()->id(), count);
  }
  else if (channel->type() == SimpleID::UserId) {
    if (!user)
      return FeedReply(Notice::BadRequest);

    messages = NodeMessagesDB::last(channel->id(), user->id(), count);
  }

  if (messages.isEmpty())
    return FeedReply(Notice::NotFound);

  FeedReply reply(Notice::OK);
  reply.json[LS("count")]    = messages.size();
  reply.json[LS("messages")] = encode(messages);
  return reply;
}


QStringList NodeMessagesFeed::encode(const QList<QByteArray> &ids)
{
  QStringList out;
  out.reserve(ids.size());
  foreach (const QByteArray &id, ids) {
    out.append(SimpleID::encode(id));
  }

  return out;
}

