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

#include "feeds/FeedsCore.h"
#include "net/packets/Notice.h"
#include "Ch.h"
#include "net/packets/FeedNotice.h"
#include "feeds/FeedEvents.h"


FeedReply FeedsCore::post(ChatChannel channel, const QString &name, ChatChannel sender, const QVariant &value, int options)
{
  QVariantMap json;
  json[FEED_KEY_VALUE]   = value;
  json[FEED_KEY_OPTIONS] = options;
  return request(channel, FEED_METHOD_POST, name, sender, json);
}


FeedReply FeedsCore::post(const QString &name, ChatChannel sender, const QVariant &value, int options)
{
  return post(Ch::server(), name, sender, value, options);
}


FeedReply FeedsCore::post(const QString &name, const QVariant &value, int options)
{
  ChatChannel server = Ch::server();
  return post(server, name, server, value, options);
}


/*!
 * Базовая функция совершения операции над фидом.
 *
 * \param channel Канал владелец фида.
 * \param method  Метод \sa Methods.
 * \param name    Имя фида с опциональным путём запроса.
 * \param sender  Канал создавший запрос.
 * \param json    Данные запроса.
 */
FeedReply FeedsCore::request(ClientChannel channel, const QString &method, const QString &name, ChatChannel sender, const QVariantMap &json)
{
  if (!channel || !sender)
    return Notice::InternalError;

  const QPair<QString, QString> split = FeedNotice::split(name);
  FeedPtr feed = channel->feed(split.first, false);
  if (!feed)
    return Notice::NotFound;

  FeedReply reply(Notice::InternalError);
  const int cmd    = methodToInt(method);
  FeedEvent *event = new FeedEvent(channel->id(), sender->id(), method);
  event->request   = json;
  event->name      = split.first;
  event->path      = split.second;

  switch (cmd) {
    case Get:
      reply = feed->get(event->path, json, sender.data());
      break;

    case Post:
      reply = feed->post(event->path, json, sender.data());
      break;

    case Put:
      reply = feed->put(event->path, json, sender.data());
      break;

    case Delete:
      reply = feed->del(event->path, sender.data());
      break;
  }

  return done(event, reply);
}


/*!
 * Преобразование текстового представления метода в число.
 *
 * \sa Methods.
 */
int FeedsCore::methodToInt(const QString &method)
{
  if (method == FEED_METHOD_GET)
    return Get;
  else if (method == FEED_METHOD_POST)
    return Post;
  else if (method == FEED_METHOD_PUT)
    return Put;
  else if (method == FEED_METHOD_DELETE)
    return Delete;

  return Unknown;
}


FeedReply FeedsCore::done(FeedEvent *event, const FeedReply &reply)
{
  event->reply  = reply.json;
  event->status = reply.status;

  FeedEvents::start(event);
  return reply;
}
