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

#ifndef NODEMESSAGESFEED_H_
#define NODEMESSAGESFEED_H_

#include "feeds/Feed.h"
#include "feeds/FeedCreator.h"
#include "net/packets/MessageNotice.h"

/*!
 * Серверная реализация фида \b messages.
 */
class NodeMessagesFeed : public Feed
{
public:
  NodeMessagesFeed(const QString &name, const QVariantMap &data);
  NodeMessagesFeed(const QString &name = FEED_NAME_MESSAGES, qint64 date = 0);

  FeedReply get(const QString &path, const QVariantMap &json = QVariantMap(), Channel *channel = 0) const;

private:
  FeedReply fetch(const QVariantMap &json, Channel *user) const;
  FeedReply last(const QVariantMap &json, Channel *user) const;
  FeedReply logging() const;
  FeedReply offline(Channel *user) const;
  FeedReply since(const QVariantMap &json, Channel *user) const;
  void toPackets(QList<QByteArray> &out, const QList<MessageRecordV2> &records) const;
};


class NodeMessagesFeedCreator : public FeedCreator
{
public:
  Feed* create(const QString &name) const;
  Feed* load(const QString &name, const QVariantMap &data) const;
  inline QString name() const { return FEED_NAME_MESSAGES; }
};

#endif /* NODEMESSAGESFEED_H_ */
