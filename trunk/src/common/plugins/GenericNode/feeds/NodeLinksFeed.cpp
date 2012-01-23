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

#include "DateTime.h"
#include "feeds/NodeLinksFeed.h"

NodeLinksFeed::NodeLinksFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  m_header.acl().setMask(0600);
}


NodeLinksFeed::NodeLinksFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  m_header.acl().setMask(0600);
}


Feed* NodeLinksFeed::create(const QString &name)
{
  return new NodeLinksFeed(name, DateTime::utc());
}


Feed* NodeLinksFeed::load(const QString &name, const QVariantMap &data)
{
  return new NodeLinksFeed(name, data);
}
