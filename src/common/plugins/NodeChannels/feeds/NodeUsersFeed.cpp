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
#include "Storage.h"

#define USERS_FEED_COUNT QLatin1String("count")
#define USERS_FEED_PEAK  QLatin1String("peak")
#define USERS_FEED_DATE  QLatin1String("date")

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
    int count = m_data.value(USERS_FEED_COUNT).toInt();
    if (SimpleID::typeOf(SimpleID::decode(path)) == SimpleID::UserId) {
      --count;
      m_data[USERS_FEED_COUNT] = count;
      return FeedReply(Notice::OK, DateTime::utc());
    }
  }

  return Notice::NotModified;
}


FeedReply NodeUsersFeed::post(const QString &path, const QVariantMap &json, Channel *channel)
{
  Q_UNUSED(json)

  if (!can(channel, Acl::Edit | Acl::SpecialEdit))
    return Notice::Forbidden;

  if (path.size() == 34) {
    int count = m_data.value(USERS_FEED_COUNT).toInt();
    if (SimpleID::typeOf(SimpleID::decode(path)) == SimpleID::UserId) {
      ++count;
      m_data[USERS_FEED_COUNT] = count;
      const qint64 date = DateTime::utc();
      const int peak    = m_data.value(USERS_FEED_PEAK).toMap().value(USERS_FEED_COUNT).toInt();
      if (count >= peak)
        setPeak(count, date);

      return FeedReply(Notice::OK, date);
    }
  }

  return Notice::NotModified;
}


void NodeUsersFeed::init()
{
  m_header.acl().setMask(0444);
  m_data[USERS_FEED_COUNT] = 0;

  if (!m_data.contains(USERS_FEED_PEAK)) {
    const QVariantMap peak = Storage::value(STORAGE_PEAK_USERS).toMap();
    if (peak.isEmpty())
      setPeak(0, head().date());
    else
      m_data[USERS_FEED_PEAK] = peak;
  }
}


void NodeUsersFeed::setPeak(int count, qint64 date)
{
  QVariantMap peak;
  peak[USERS_FEED_COUNT]  = count;
  peak[USERS_FEED_DATE]   = date;
  m_data[USERS_FEED_PEAK] = peak;
}


Feed* NodeUsersFeedCreator::create(const QString &name) const
{
  return new NodeUsersFeed(name, DateTime::utc());
}


Feed* NodeUsersFeedCreator::load(const QString &name, const QVariantMap &data) const
{
  return new NodeUsersFeed(name, data);
}
