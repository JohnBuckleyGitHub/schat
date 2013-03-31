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
#include "feeds/ChannelFeed.h"
#include "feeds/NodeChannelFeed.h"
#include "net/SimpleID.h"

NodeChannelFeed::NodeChannelFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
}


NodeChannelFeed::NodeChannelFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  m_header.acl().setMask(0644);
}


void NodeChannelFeed::setChannel(Channel *channel)
{
  Feed::setChannel(channel);

  m_data[CHANNEL_FEED_NAME_KEY]   = channel->name();
  m_data[CHANNEL_FEED_GENDER_KEY] = channel->gender().raw();
  m_data[CHANNEL_FEED_STATUS_KEY] = channel->status().value();
  m_data[CHANNEL_FEED_TYPE_KEY]   = SimpleID::typeOf(channel->id());
}


Feed* NodeChannelFeedCreator::create(const QString &name) const
{
  return new NodeChannelFeed(name, DateTime::utc());
}


Feed* NodeChannelFeedCreator::load(const QString &name, const QVariantMap &data) const
{
  return new NodeChannelFeed(name, data);
}
