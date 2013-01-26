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

#include "Ch.h"
#include "feeds/FeedEvents.h"
#include "feeds/FeedsCore.h"
#include "feeds/FeedStorage.h"
#include "net/packets/FeedNotice.h"
#include "net/packets/Notice.h"
#include "Sockets.h"

FeedReply FeedsCore::post(const QString &name, const QVariant &value, int options)
{
  ServerChannel *server = Ch::server().data();
  return post(server, name, server, value, options);
}


FeedReply FeedsCore::post(const QString &name, ServerChannel *sender, const QVariant &value, int options)
{
  return post(Ch::server().data(), name, sender, value, options);
}


FeedReply FeedsCore::post(ServerChannel *channel, const QString &name, ServerChannel *sender, const QVariant &value, int options)
{
  QVariantMap json;
  json[FEED_KEY_VALUE]   = value;
  json[FEED_KEY_OPTIONS] = options;
  return request(channel, FEED_METHOD_POST, name, sender, json);
}


FeedReply FeedsCore::put(const QString &name, const QVariant &value, int options)
{
  ServerChannel *server = Ch::server().data();
  return put(server, name, server, value, options);
}


FeedReply FeedsCore::put(const QString &name, ServerChannel *sender, const QVariant &value, int options)
{
  return put(Ch::server().data(), name, sender, value, options);
}


FeedReply FeedsCore::put(ServerChannel *channel, const QString &name, ServerChannel *sender, const QVariant &value, int options)
{
  QVariantMap json;
  json[FEED_KEY_VALUE]   = value;
  json[FEED_KEY_OPTIONS] = options;
  return request(channel, FEED_METHOD_PUT, name, sender, json);
}


FeedReply FeedsCore::del(const QString &name)
{
  ServerChannel *server = Ch::server().data();
  return del(server, name, server);
}


FeedReply FeedsCore::del(const QString &name, ServerChannel *sender)
{
  return del(Ch::server().data(), name, sender);
}


FeedReply FeedsCore::del(ServerChannel *channel, const QString &name, ServerChannel *sender)
{
  return request(channel, FEED_METHOD_DELETE, name, sender);
}


FeedReply FeedsCore::get(const QString &name, const QVariantMap &data)
{
  ServerChannel *server = Ch::server().data();
  return get(server, name, server, data);
}


FeedReply FeedsCore::get(const QString &name, ServerChannel *sender, const QVariantMap &data)
{
  return get(Ch::server().data(), name, sender, data);
}


FeedReply FeedsCore::get(ServerChannel *channel, const QString &name, ServerChannel *sender, const QVariantMap &data)
{
  return request(channel, FEED_METHOD_GET, name, sender, data);
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
FeedReply FeedsCore::request(ServerChannel *channel, const QString &method, const QString &name, ServerChannel *sender, const QVariantMap &json)
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
  event->date      = feed->head().date();

  if (!feed->can(sender, Acl::Read))
    done(event, Notice::Forbidden);

  if (cmd != Get && !feed->can(sender, Acl::Write))
    done(event, Notice::Forbidden);

  switch (cmd) {
    case Get:
      reply = feed->get(event->path, json, sender);
      break;

    case Post:
      reply = feed->post(event->path, json, sender);
      break;

    case Put:
      reply = feed->put(event->path, json, sender);
      break;

    case Delete:
      reply = feed->del(event->path, sender);
      break;
  }

  if (cmd != Get && reply.status == Notice::OK) {
    event->diffTo = event->date;
    event->date   = reply.date;

    if (reply.date)
      FeedStorage::save(feed, reply.date);

    if (channel->type() != SimpleID::ServerId)
      event->broadcast = Sockets::channel(channel);
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
