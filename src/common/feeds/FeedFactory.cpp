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

#include "feeds/FeedFactory.h"

QHash<QString, FeedPtr> FeedFactory::m_feeds;


Feed* FeedFactory::create(const QString &name)
{
  if (!m_feeds.contains(name)) {
    Feed feed;
    return feed.create(name);
  }

  return m_feeds.value(name)->create(name);
}


/*!
 * Восстановление фида из JSON данных.
 */
Feed* FeedFactory::load(const QString &name, const QVariantMap &data)
{
  if (!m_feeds.contains(name)) {
    Feed feed;
    return feed.load(name, data);
  }

  return m_feeds.value(name)->load(name, data);
}
