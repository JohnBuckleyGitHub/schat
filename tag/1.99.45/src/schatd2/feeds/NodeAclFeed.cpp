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
#include "feeds/NodeAclFeed.h"
#include "net/packets/Notice.h"
#include "sglobal.h"

NodeAclFeed::NodeAclFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
}


NodeAclFeed::NodeAclFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  m_header.acl().setMask(0766);
}


Feed* NodeAclFeed::create(const QString &name)
{
  return new NodeAclFeed(name, DateTime::utc());
}


Feed* NodeAclFeed::load(const QString &name, const QVariantMap &data)
{
  return new NodeAclFeed(name, data);
}


FeedReply NodeAclFeed::del(const QString &path, Channel *channel)
{
  if (path.startsWith(LS("head/")))
    return Feed::del(path, channel);

  return Notice::Forbidden;
}


FeedReply NodeAclFeed::post(const QString &path, const QVariantMap &json, Channel *channel)
{
  if (path.startsWith(LS("head/")))
    return Feed::post(path, json, channel);

  return Notice::Forbidden;
}


FeedReply NodeAclFeed::put(const QString &path, const QVariantMap &json, Channel *channel)
{
  if (path.startsWith(LS("head/")))
    return Feed::put(path, json, channel);

  return Notice::Forbidden;
}
