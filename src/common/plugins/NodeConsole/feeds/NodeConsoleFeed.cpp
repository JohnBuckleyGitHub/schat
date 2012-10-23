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


Feed* NodeConsoleFeed::create(const QString &name)
{
  return new NodeConsoleFeed(name, DateTime::utc());
}


Feed* NodeConsoleFeed::load(const QString &name, const QVariantMap &data)
{
  return new NodeConsoleFeed(name, data);
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

  return FeedReply(Notice::NotFound);
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
    return FeedReply(Notice::BadRequest);

  QString password = json.value(LS("password")).toString();
  if (password.isEmpty() || SimpleID::typeOf(SimpleID::decode(password)) != SimpleID::PasswordId)
    return FeedReply(Notice::BadRequest);

  if (Storage::value(LS("password")) != password)
    return FeedReply(Notice::Forbidden);

  user->account()->groups.add(LS("master"));
  user->account()->setDate(DateTime::utc());
  DataBase::add(Ch::channel(user->id(), SimpleID::typeOf(user->id()), false));

  return FeedReply(Notice::OK);
}


/*!
 * Проверка привилегий пользователя, только пользователь в группе \b master имеет права доступа к фиду.
 */
FeedReply NodeConsoleFeed::tryAccess(Channel *user) const
{
  if (!master(user))
    return FeedReply(Notice::Forbidden);

  return FeedReply(Notice::OK);
}
