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

#ifndef NODECHANNELFEED_H_
#define NODECHANNELFEED_H_

#include "feeds/Feed.h"
#include "feeds/FeedCreator.h"

/*!
 * Серверная реализация фида \b channel.
 */
class NodeChannelFeed : public Feed
{
public:
  NodeChannelFeed(const QString &name, const QVariantMap &data);
  NodeChannelFeed(const QString &name = FEED_NAME_CHANNEL, qint64 date = 0);

  void setChannel(Channel *channel);
};


class NodeChannelFeedCreator : public FeedCreator
{
public:
  Feed* create(const QString &name) const;
  Feed* load(const QString &name, const QVariantMap &data) const;
  inline QString name() const { return FEED_NAME_CHANNEL; }
};

#endif /* NODECHANNELFEED_H_ */
