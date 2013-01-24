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

#ifndef FEEDSCORE_H_
#define FEEDSCORE_H_

#include <QVariant>

#include "feeds/Feed.h"
#include "ServerChannel.h"

class FeedEvent;

class SCHAT_EXPORT FeedsCore
{
  FeedsCore() {}

public:
  static FeedReply post(ChatChannel channel, const QString &name, ChatChannel sender, const QVariant &value = QVariant(), int options = 0);
  static FeedReply post(const QString &name, ChatChannel sender, const QVariant &value = QVariant(), int options = 0);
  static FeedReply post(const QString &name, const QVariant &value = QVariant(), int options = 0);

  static FeedReply request(ClientChannel channel, const QString &method, const QString &name, ChatChannel sender, const QVariantMap &json = QVariantMap());

private:
  /// Числовое представление методов.
  enum Methods {
    Unknown,
    Get,
    Post,
    Put,
    Delete
  };

  static int methodToInt(const QString &method);
  static FeedReply done(FeedEvent *event, const FeedReply &reply);
};

#endif /* FEEDSCORE_H_ */
