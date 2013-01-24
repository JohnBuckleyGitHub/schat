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

#include "Channel.h"
#include "DateTime.h"
#include "feeds/NodeUsersFeed.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "sglobal.h"

NodeUsersFeed::NodeUsersFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  init();
}


NodeUsersFeed::NodeUsersFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  init();
}


FeedReply NodeUsersFeed::del(const QString &path, Channel *channel)
{
  if (!can(channel, Acl::Edit | Acl::SpecialEdit))
    return Notice::Forbidden;

  if (path.size() == 34) {
    int count = m_data.value(LS("count")).toInt();
    if (SimpleID::typeOf(SimpleID::decode(path)) == SimpleID::UserId) {
      --count;
      m_data[LS("count")] = count;
      return FeedReply(Notice::OK, DateTime::utc());
    }
  }

  return Notice::NotModified;
}


FeedReply NodeUsersFeed::post(const QString &path, const QVariantMap &json, Channel *channel)
{
  if (!can(channel, Acl::Edit | Acl::SpecialEdit))
    return Notice::Forbidden;

  if (path.size() == 34) {
    int count = m_data.value(LS("count")).toInt();
    if (SimpleID::typeOf(SimpleID::decode(path)) == SimpleID::UserId) {
      ++count;
      m_data[LS("count")] = count;
      return FeedReply(Notice::OK, DateTime::utc());
    }
  }

  return Notice::NotModified;
}


void NodeUsersFeed::init()
{
  m_header.acl().setMask(0444);
  m_data[LS("count")] = 0;
}


Feed* NodeUsersFeedCreator::create(const QString &name) const
{
  return new NodeUsersFeed(name, DateTime::utc());
}


Feed* NodeUsersFeedCreator::load(const QString &name, const QVariantMap &data) const
{
  return new NodeUsersFeed(name, data);
}
