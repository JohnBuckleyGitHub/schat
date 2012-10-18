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
#include "Channel.h"
#include "DateTime.h"
#include "feeds/NodeConsoleFeed.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "Storage.h"

NodeConsoleFeed::NodeConsoleFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  init();
}


NodeConsoleFeed::NodeConsoleFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  init();
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
FeedReply NodeConsoleFeed::get(const QString &path, const QVariantMap &json, Channel *channel)
{
  if (path == LS("login"))
    return login(json, channel);
  else if (path == LS("try"))
    return tryAccess(json, channel);

  return FeedReply(Notice::NotImplemented);
}


/*!
 * Этот фид не имеет тела.
 */
QVariantMap NodeConsoleFeed::feed(Channel *channel)
{
  Q_UNUSED(channel)
  return QVariantMap();
}


/*!
 * \return \b true если пользователь входит в группу \b master.
 */
bool NodeConsoleFeed::master(Channel *user)
{
  if (user && user->account()->groups.contains(LS("master")) && Storage::value(LS("password")) != LS("2AZ6EKXDJCXLKZQPYIKAV3BVQUGE3KMXOA"))
    return true;

  return false;
}


/*!
 * Проверка пароля сервера.
 */
FeedReply NodeConsoleFeed::login(const QVariantMap &json, Channel *user)
{
  if (!user)
    return FeedReply(Notice::BadRequest);

  QString password = json.value(LS("password")).toString();
  if (password.isEmpty() || SimpleID::typeOf(SimpleID::decode(password)) != SimpleID::PasswordId)
    return FeedReply(Notice::BadRequest);

  if (Storage::value(LS("password")) != password)
    return FeedReply(Notice::Forbidden);

  user->account()->groups.add(LS("master"));
  return FeedReply(Notice::OK);
}


/*!
 * Проверка привилегий пользователя, только пользователь в группе \b master имеет права доступа к фиду.
 */
FeedReply NodeConsoleFeed::tryAccess(const QVariantMap &json, Channel *user)
{
  Q_UNUSED(json)
  if (!master(user))
    return FeedReply(Notice::Forbidden);

  return FeedReply(Notice::OK);
}


void NodeConsoleFeed::init()
{
  m_header.acl().setMask(0444);

  if (Storage::value(LS("password")).toString().size() != 34)
    Storage::setValue(LS("password"), LS("2AZ6EKXDJCXLKZQPYIKAV3BVQUGE3KMXOA"));
}
