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
#include "sglobal.h"

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
FeedReply NodeConsoleFeed::get(const QString &path, const QVariantMap &json, Channel *channel)
{
  if (path == LS("try"))
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
  if (user && user->account()->groups.all().contains(LS("master")))
    return true;

  return false;
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
