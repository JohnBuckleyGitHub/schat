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

#ifndef NODESTORAGEFEED_H_
#define NODESTORAGEFEED_H_

#include "feeds/Feed.h"

class NodeStorageFeed : public Feed
{
public:
  NodeStorageFeed(const QString &name, const QVariantMap &data);
  NodeStorageFeed(const QString &name = QLatin1String("storage"), qint64 date = 0);
  Feed* create(const QString &name);
  Feed* load(const QString &name, const QVariantMap &data);

  FeedReply get(const QString &path, const QVariantMap &json = QVariantMap(), Channel *channel = 0) const;
  FeedReply post(const QString &path, const QVariantMap &json, Channel *channel = 0);
  FeedReply put(const QString &path, const QVariantMap &json, Channel *channel = 0);
};

#endif /* NODESTORAGEFEED_H_ */
