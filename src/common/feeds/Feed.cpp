/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#include <QDateTime>

#include "feeds/Feed.h"
#include "net/SimpleID.h"


/*!
 * Конструктор создающий пустой объект.
 */
FeedHeader::FeedHeader()
  : m_date(0)
{
}


QVariantMap FeedHeader::json(ClientUser user) const
{
  QVariantMap out;
  int acl = m_acl.acl();
  if (user) {
    acl = m_acl.match(user);
    if (!acl)
      return out;
  }

  out["acl"]  = acl;
  out["date"] = m_date;

  return out;
}


qint64 FeedHeader::timestamp()
{
# if QT_VERSION >= 0x040700
  return QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
# else
  QDateTime dt = QDateTime::currentDateTime().toUTC();
  qint64 t = dt.toTime_t();
  t *= 1000;
  t += dt.time().msec();
  return t;
# endif
}


bool FeedHeader::isValid() const
{
  if (m_name.isEmpty())
    return false;

  if (SimpleID::typeOf(m_id) != SimpleID::ChannelId)
    return false;

  return true;
}


Feed::Feed()
{
}


Feed::Feed(const QString &name, qint64 date)
{
  m_header.setName(name);
  m_header.setDate(date);
}


/*!
 * Возвращает \b true если данные фида корректны.
 */
bool Feed::isValid() const
{
  return m_header.isValid();
}


bool Feeds::add(FeedPtr feed)
{
  if (!feed)
    return false;

  if (!feed->isValid())
    return false;

  m_feeds[feed->h().name()] = feed;
  return true;
}


QVariantMap Feeds::headers(ClientUser user) const
{
  QVariantMap map;

  QHashIterator<QString, FeedPtr> i(m_feeds);
  while (i.hasNext()) {
    i.next();
    QVariantMap header = i.value()->h().json(user);
    if (!header.isEmpty()) {
      map[i.key()] = header;
    }
  }

  return map;
}


bool Feeds::merge(const QString &key, QVariantMap &out, const QVariantMap &in)
{
  if (in.isEmpty())
    return false;

  out[key] = in;
  return true;
}


QVariantMap Feeds::merge(const QString &key, const QVariantMap &in)
{
  QVariantMap out;
  if (!in.isEmpty())
    out[key] = in;

  return out;
}
