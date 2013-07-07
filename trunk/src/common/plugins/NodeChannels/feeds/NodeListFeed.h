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

#ifndef NODELISTFEED_H_
#define NODELISTFEED_H_

#include "feeds/Feed.h"
#include "feeds/FeedCreator.h"

/*!
 * Серверная реализация фида \b list.
 */
class NodeListFeed : public Feed
{
public:
  NodeListFeed(const QString &name, const QVariantMap &data);
  NodeListFeed(const QString &name = FEED_NAME_LIST, qint64 date = 0);

  FeedReply del(const QString &path, Channel *channel = 0, const QByteArray &blob = QByteArray());
  FeedReply get(const QString &path, const QVariantMap &json = QVariantMap(), Channel *channel = 0, const QByteArray &blob = QByteArray()) const;
  FeedReply post(const QString &path, const QVariantMap &json, Channel *channel = 0, const QByteArray &blob = QByteArray());
  FeedReply put(const QString &path, const QVariantMap &json, Channel *channel = 0, const QByteArray &blob = QByteArray());

private:
  FeedReply id(const QString &name) const;
  void init();
};


class NodeListFeedCreator : public FeedCreator
{
public:
  Feed* create(const QString &name) const;
  Feed* load(const QString &name, const QVariantMap &data) const;
  inline QString name() const { return FEED_NAME_LIST; }
};

#endif /* NODELISTFEED_H_ */
