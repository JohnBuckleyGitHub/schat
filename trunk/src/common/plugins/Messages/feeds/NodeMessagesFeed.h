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

#ifndef NODEMESSAGESFEED_H_
#define NODEMESSAGESFEED_H_

#include "feeds/Feed.h"
#include "net/packets/MessageNotice.h"

class NodeMessagesFeed : public Feed
{
public:
  NodeMessagesFeed(const QString &name, const QVariantMap &data);
  NodeMessagesFeed(const QString &name = QLatin1String("messages"), qint64 date = 0);
  Feed* create(const QString &name);
  Feed* load(const QString &name, const QVariantMap &data);

  FeedReply get(const QString &path, const QVariantMap &json = QVariantMap(), Channel *channel = 0);

private:
  FeedReply fetch(const QVariantMap &json, Channel *user);
  FeedReply last(const QVariantMap &json, Channel *user);
  FeedReply offline(Channel *user);
  FeedReply since(const QVariantMap &json, Channel *user);
  void toPackets(QList<QByteArray> &out, const QList<MessageRecord> &records) const;
};

#endif /* NODEMESSAGESFEED_H_ */
