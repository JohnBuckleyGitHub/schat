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

#ifndef NODEPROFILEFEED_H_
#define NODEPROFILEFEED_H_

#include "feeds/Feed.h"

class NodeProfileFeed : public Feed
{
public:
  NodeProfileFeed(const QString &name, const QVariantMap &data);
  NodeProfileFeed(const QString &name = QLatin1String("profile"), qint64 date = 0);
  Feed* create(const QString &name);
  Feed* load(const QString &name, const QVariantMap &data);

  QVariantMap feed(Channel *channel = 0) const;

protected:
//  FeedQueryReply set(const QVariantMap &json, Channel *channel);
};

#endif /* NODEPROFILEFEED_H_ */
