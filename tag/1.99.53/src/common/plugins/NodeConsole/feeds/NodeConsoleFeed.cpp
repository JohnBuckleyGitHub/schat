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

#include "Account.h"
#include "Ch.h"
#include "Channel.h"
#include "DataBase.h"
#include "DateTime.h"
#include "feeds/NodeConsoleFeed.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "Storage.h"

NodeConsoleFeed::NodeConsoleFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  m_header.acl().setMask(0444);
}


NodeConsoleFeed::NodeConsoleFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  m_header.acl().setMask(0444);
}


FeedReply NodeConsoleFeed::del(const QString &path, Channel *channel)
{
  if (path.isEmpty() || !channel)
    return Notice::BadRequest;

  if (path == LS("me")) {
    if (!master(channel))
      return Notice::NotModified;

    channel->account()->groups.remove(LS("master"));
    channel->account()->setDate(DateTime::utc());
    DataBase::add(Ch::channel(channel->id(), SimpleID::typeOf(channel->id()), false));

    return FeedReply(Notice::OK, DateTime::utc());
  }
  else if (path.size() == 34) {
    QByteArray id = SimpleID::decode(path);
    if (SimpleID::typeOf(id) != SimpleID::UserId)
      return Notice::BadRequest;

    ChatChannel user = Ch::channel(id, SimpleID::UserId);
    if (!user)
      return Notice::BadRequest;

    if (!user->account()->groups.contains(LS("master")))
      return Notice::NotModified;

    user->account()->groups.remove(LS("master"));
    user->account()->setDate(DateTime::utc());
    DataBase::add(user);

    return FeedReply(Notice::OK, DateTime::utc());
  }

  return Notice::NotFound;
}


/*!
 * Обработка \b get запросов.
 */
FeedReply NodeConsoleFeed::get(const QString &path, const QVariantMap &json, Channel *channel) const
{
  if (path.isEmpty())
    return Notice::BadRequest;

  if (path == LS("head"))
    return FeedReply(Notice::OK, head().get(channel), head().date());

  if (path == LS("login"))
    return login(json, channel);
  else if (path == LS("try"))
    return tryAccess(channel);

  return Notice::NotFound;
}


/*!
 * \return \b true если пользователь входит в группу \b master.
 */
bool NodeConsoleFeed::master(Channel *user) const
{
  if (user && user->account()->groups.contains(LS("master")) && Storage::value(LS("password")) != LS("2AZ6EKXDJCXLKZQPYIKAV3BVQUGE3KMXOA"))
    return true;

  return false;
}


/*!
 * Проверка пароля сервера.
 */
FeedReply NodeConsoleFeed::login(const QVariantMap &json, Channel *user) const
{
  if (!user)
    return Notice::BadRequest;

  QString password = json.value(LS("password")).toString();
  if (password.isEmpty() || SimpleID::typeOf(SimpleID::decode(password)) != SimpleID::PasswordId)
    return Notice::BadRequest;

  if (Storage::value(LS("password")) != password)
    return Notice::Forbidden;

  user->account()->groups.add(LS("master"));
  user->account()->setDate(DateTime::utc());
  DataBase::add(Ch::channel(user->id(), SimpleID::typeOf(user->id()), false));

  return FeedReply(Notice::OK, DateTime::utc());
}


/*!
 * Проверка привилегий пользователя, только пользователь в группе \b master имеет права доступа к фиду.
 */
FeedReply NodeConsoleFeed::tryAccess(Channel *user) const
{
  if (!master(user))
    return Notice::Forbidden;

  return FeedReply(Notice::OK, DateTime::utc());
}


Feed* NodeConsoleFeedCreator::create(const QString &name) const
{
  return new NodeConsoleFeed(name, DateTime::utc());
}


Feed* NodeConsoleFeedCreator::load(const QString &name, const QVariantMap &data) const
{
  return new NodeConsoleFeed(name, data);
}
