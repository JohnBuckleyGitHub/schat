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
#include "feeds/NodeStatsFeed.h"
#include "feeds/StatsFeed.h"
#include "ServerChannel.h"

NodeStatsFeed::NodeStatsFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  init();
}


NodeStatsFeed::NodeStatsFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  init();
}


void NodeStatsFeed::setChannel(Channel *channel)
{
  Feed::setChannel(channel);

  if ((channel->type() == ChatId::ServerId || channel->type() == ChatId::ChannelId) && !m_data.contains(STATS_FEED_USERS_KEY)) {
    m_data.insert(STATS_FEED_USERS_KEY, QVariantList() << 0 << 0 << 0);
  }
}


void NodeStatsFeed::init()
{
  m_header.acl().setMask(0444);
}


Feed* NodeStatsFeedCreator::create(const QString &name) const
{
  return new NodeStatsFeed(name, DateTime::utc());
}


Feed* NodeStatsFeedCreator::load(const QString &name, const QVariantMap &data) const
{
  return new NodeStatsFeed(name, data);
}
