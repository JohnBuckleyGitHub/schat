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


FeedReply NodeProfileFeed::post(const QString &path, const QVariantMap &json, Channel *channel)
{
  Q_UNUSED(channel);

  if (path.isEmpty() || path.contains(LC('*')) || !json.contains(LS("value")))
    return Notice::BadRequest;

  const QVariant& value = json[LS("value")];
  User *user = static_cast<ServerChannel *>(head().channel())->user();

  if (user->set(path, value)) {
    DataBase::add(user);
    return FeedReply(Notice::OK, DateTime::utc());
  }

  return Notice::NotModified;
}


FeedReply NodeProfileFeed::put(const QString &path, const QVariantMap &json, Channel *channel)
{
  Q_UNUSED(path)
  Q_UNUSED(json)
  Q_UNUSED(channel)
  return Notice::Forbidden;
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


Feed* NodeProfileFeedCreator::create(const QString &name) const
{
  return new NodeProfileFeed(name, DateTime::utc());
}


Feed* NodeProfileFeedCreator::load(const QString &name, const QVariantMap &data) const
{
  return new NodeProfileFeed(name, data);
}
