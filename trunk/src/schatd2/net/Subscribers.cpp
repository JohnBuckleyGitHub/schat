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

#include "DateTime.h"
#include "net/Subscribers.h"

Subscribers::Subscribers()
{
}


qint64 Subscribers::contains(const QString &path, const ChatId &id) const
{
  if (!m_map.contains(path))
    return 0;

  const Ids& ids = m_map[path];
  if (!ids.contains(id))
    return 0;

  return ids.value(id);
}


void Subscribers::add(const QString &path, const ChatId &id)
{
  m_map[path].insert(id, DateTime::utc());
}
