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

#ifndef NODEUSERFEED_H_
#define NODEUSERFEED_H_

#include "feeds/Feed.h"

class NodeUserFeed : public Feed
{
public:
  NodeUserFeed(const QString &name, const QVariantMap &data);
  NodeUserFeed(const QString &name = FEED_NAME_USER, qint64 date = 0);
  Feed* create(const QString &name);
  Feed* load(const QString &name, const QVariantMap &data);

  QVariantMap feed(Channel *channel = 0) const;
};

#endif /* NODEUSERFEED_H_ */
