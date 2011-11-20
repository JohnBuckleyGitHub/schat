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

#include "feeds/Feeds.h"

bool Feeds::add(FeedPtr feed)
{
  if (!feed)
    return false;

  feed->h().setId(m_id);

  if (!feed->isValid())
    return false;

  m_feeds[feed->h().name()] = feed;
  return true;
}


QVariantMap Feeds::json(ClientUser user, bool body)
{
  return json(m_feeds.keys(), user, body);
}


QVariantMap Feeds::json(const QStringList &feeds, ClientUser user, bool body)
{
  if (feeds.isEmpty())
    return QVariantMap();

  QVariantMap json;
  for (int i = 0; i < feeds.size(); ++i) {
    FeedPtr feed = m_feeds.value(feeds.at(i));
    if (!feed)
      continue;

    bool b = body;

    if (user) {
      int acl = feed->h().acl().match(user);
      if (!acl)
        continue;

      if (!(acl & Acl::Read))
        b = false;
    }

    QVariantMap current;
    if (b)
      current = feed->json();

    if (!feed->h().json(current, user))
      continue;

    json[feeds.at(i)] = current;
  }

  return merge("feeds", json);
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
