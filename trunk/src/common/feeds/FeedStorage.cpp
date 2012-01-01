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

#include "feeds/FeedStorage.h"
#include "net/packets/Notice.h"

FeedStorage *FeedStorage::m_self = 0;

FeedStorage::FeedStorage(QObject *parent)
  : QObject(parent)
{
  if (!m_self)
    m_self = this;
  else
    add(this);
}


int FeedStorage::saveImpl(FeedPtr feed)
{
  if (m_hooks.isEmpty())
    return Notice::OK;

  foreach (FeedStorage *hook, m_hooks) {
    int status = hook->saveImpl(feed);
    if (status != Notice::OK)
      return status;
  }

  return Notice::OK;
}
