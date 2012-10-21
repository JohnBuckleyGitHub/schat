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

#include "DataBase.h"
#include "DateTime.h"
#include "feeds/NodeProfileFeed.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "ServerChannel.h"
#include "sglobal.h"
#include "User.h"

NodeProfileFeed::NodeProfileFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  m_header.acl().setMask(0744);
}


NodeProfileFeed::NodeProfileFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  m_header.acl().setMask(0744);
}


Feed* NodeProfileFeed::create(const QString &name)
{
  return new NodeProfileFeed(name, DateTime::utc());
}


Feed* NodeProfileFeed::load(const QString &name, const QVariantMap &data)
{
  return new NodeProfileFeed(name, data);
}


QVariantMap NodeProfileFeed::feed(Channel *channel) const
{
  if (head().channel()->type() != SimpleID::UserId || !Acl::canRead(this, channel))
    return QVariantMap();

  User *user = static_cast<ServerChannel *>(head().channel())->user();
  QVariantMap out = user->toMap();

  Account *account = head().channel()->account();
  if (!account->provider.isEmpty())
    out[LS("provider")] = account->provider;

  return out;
}


/*!
 * Альтернативная обработка запроса "x-set", для непосредственной модификации объекта User.
 *
 * \bug Исправить это.
 */
//FeedQueryReply NodeProfileFeed::set(const QVariantMap &json, Channel *channel)
//{
//  if (head().channel()->type() != SimpleID::UserId)
//    return FeedQueryReply(Notice::InternalError);
//
//  if (!Acl::canWrite(this, channel))
//    return FeedQueryReply(Notice::Forbidden);
//
//  QStringList keys = json.keys();
//  keys.removeAll(LS("action"));
//  if (keys.isEmpty())
//    return FeedQueryReply(Notice::BadRequest);
//
//  User *user = static_cast<ServerChannel *>(head().channel())->user();
//
//  FeedQueryReply reply(Notice::OK);
//  reply.incremental = true;
//  reply.json[LS("action")] = LS("x-set");
//
//  foreach (QString key, keys) {
//    QVariant value = json.value(key);
//    if (user->set(key, value))
//      reply.json[key] = value;
//  }
//
//  if (!user->saved) {
//    reply.date = user->date;
//    reply.modified = true;
//    DataBase::add(user);
//  }
//
//  return reply;
//}
