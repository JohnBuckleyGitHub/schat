/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2014 IMPOMEZIA <schat@impomezia.com>
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

#ifndef NODECONSOLEFEED_H_
#define NODECONSOLEFEED_H_

#include "feeds/Feed.h"
#include "feeds/FeedCreator.h"
#include "ServerChannel.h"

/*!
 * Серверная реализация фида \b console.
 */
class NodeConsoleFeed : public Feed
{
public:
  NodeConsoleFeed(const QString &name, const QVariantMap &data);
  NodeConsoleFeed(const QString &name = FEED_NAME_CONSOLE, qint64 date = 0);

  FeedReply del(const QString &path, Channel *channel = 0, const QByteArray &blob = QByteArray());
  FeedReply get(const QString &path, const QVariantMap &json = QVariantMap(), Channel *channel = 0, const QByteArray &blob = QByteArray()) const;

private:
  bool master(Channel *user) const;
  FeedReply cookie(const QVariantMap &json, Channel *user) const;
  FeedReply login(const QVariantMap &json, Channel *user) const;
  FeedReply tryAccess(Channel *user) const;
  qint64 del(ChatChannel user);
};


class NodeConsoleFeedCreator : public FeedCreator
{
public:
  Feed* create(const QString &name) const;
  Feed* load(const QString &name, const QVariantMap &data) const;
  inline QString name() const { return FEED_NAME_CONSOLE; }
};

#endif /* NODECONSOLEFEED_H_ */
