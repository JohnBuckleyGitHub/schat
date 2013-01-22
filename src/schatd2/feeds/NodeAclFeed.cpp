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
#include "acl/AclValue.h"
#include "feeds/NodeAclFeed.h"
#include "net/packets/Notice.h"
#include "sglobal.h"

NodeAclFeed::NodeAclFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  init();
}


NodeAclFeed::NodeAclFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  init();
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


void NodeAclFeed::init()
{
  if (!m_data.isEmpty())
    return;

  // Импортирование существующих прав доступа.
  const QVariantMap &data = head().data();
  m_data[FEED_WILDCARD_ASTERISK] = AclValue::toByteArray(head().acl().mask() & ~0770);

  const QVariantMap others = data.value(ACL_OTHERS).toMap();
  if (!others.isEmpty()) {
    QMapIterator<QString, QVariant> i(others);
    while (i.hasNext()) {
      i.next();
      m_data[i.key()] = AclValue::toByteArray(i.value().toInt());
    }
  }

  const QStringList owners = data.value(ACL_OWNERS).toStringList();
  if (!owners.isEmpty()) {
    const QString mask = AclValue::toByteArray(head().acl().mask() >> 6 | Acl::SpecialEdit);
    foreach (const QString &id, owners)
      m_data[id] = mask;
  }
}


Feed* NodeAclFeedCreator::create(const QString &name) const
{
  return new NodeAclFeed(name, DateTime::utc());
}


Feed* NodeAclFeedCreator::load(const QString &name, const QVariantMap &data) const
{
  return new NodeAclFeed(name, data);
}
