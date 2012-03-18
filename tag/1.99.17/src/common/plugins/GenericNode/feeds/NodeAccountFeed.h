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

#ifndef NODEACCOUNTFEED_H_
#define NODEACCOUNTFEED_H_

#include "feeds/Feed.h"

class NodeAccountFeed : public Feed
{
public:
  NodeAccountFeed(const QString &name, const QVariantMap &data);
  NodeAccountFeed(const QString &name = QLatin1String("account"), qint64 date = 0);
  Feed* create(const QString &name);
  Feed* load(const QString &name, const QVariantMap &data);

  FeedQueryReply query(const QVariantMap &json, Channel *channel = 0);
  QVariantMap feed(Channel *channel = 0);
  void setChannel(Channel *channel);

private:
  FeedQueryReply login(const QVariantMap &json);
  FeedQueryReply password(const QVariantMap &json);
  FeedQueryReply reg(const QVariantMap &json);
  FeedQueryReply reset(const QVariantMap &json);

  QByteArray getPassword(const QVariantMap &json, const QString &key = QLatin1String("pass")) const;
  QString getName(const QVariantMap &json) const;
  void setRecovery(const QString &type, const QVariantMap &json);
};

#endif /* NODEACCOUNTFEED_H_ */
