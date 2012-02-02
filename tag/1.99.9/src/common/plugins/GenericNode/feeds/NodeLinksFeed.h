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

#ifndef NODELINKSFEED_H_
#define NODELINKSFEED_H_

#include "feeds/Feed.h"
#include "ServerChannel.h"

class AuthRequest;

class NodeLinksFeed : public Feed
{
public:
  NodeLinksFeed(const QString &name, const QVariantMap &data);
  NodeLinksFeed(const QString &name = QLatin1String("links"), qint64 date = 0);
  Feed* create(const QString &name);
  Feed* load(const QString &name, const QVariantMap &data);

  FeedQueryReply query(const QVariantMap &json, Channel *channel = 0);
  QVariantMap feed(Channel *channel = 0);

  static void add(ChatChannel channel, const AuthRequest &data, const QString &host);

private:
  FeedQueryReply add(const QVariantMap &json, Channel *channel);
};

#endif /* NODELINKSFEED_H_ */
