/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#ifndef FEEDS_H_
#define FEEDS_H_

#include "feeds/Feed.h"

class Channel;

/*!
 * Хранилище фидов.
 */
class SCHAT_EXPORT Feeds
{
public:
  Feeds() {}
  bool add(FeedPtr feed);
  inline bool add(Feed *feed) { return add(FeedPtr(feed)); }
  inline const QByteArray& id() const { return m_id; }
  inline const QHash<QString, FeedPtr>& all() const { return m_feeds; }
  inline void setId(const QByteArray &id) { m_id = id; }
  inline void setChannel(Channel *channel) { m_channel = channel; }

  QVariantMap json(ClientUser user = ClientUser(), bool body = true);
  QVariantMap json(const QStringList &feeds, ClientUser user = ClientUser(), bool body = true);

  static bool merge(const QString &key, QVariantMap &out, const QVariantMap &in);
  static QVariantMap merge(const QString &key, const QVariantMap &in);

private:
  Channel *m_channel;              ///< Канал фидов.
  QByteArray m_id;                 ///< Идентификатор канала.
  QHash<QString, FeedPtr> m_feeds; ///< Таблица фидов.
};

#endif /* FEEDS_H_ */
