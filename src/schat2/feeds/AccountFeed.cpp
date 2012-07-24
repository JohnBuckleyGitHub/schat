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
#include "feeds/AccountFeed.h"
#include "sglobal.h"

AccountFeed::AccountFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
}


AccountFeed::AccountFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
}


Feed* AccountFeed::create(const QString &name)
{
  return new AccountFeed(name, DateTime::utc());
}


Feed* AccountFeed::load(const QString &name, const QVariantMap &data)
{
  return new AccountFeed(name, data);
}


void AccountFeed::setChannel(Channel *channel)
{
  Feed::setChannel(channel);
  channel->account()->groups.set(m_data.value(LS("groups")).toStringList());
}
