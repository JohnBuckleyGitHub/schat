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

#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <QList>
#include <QSharedPointer>
#include <QVariant>

#include "feeds/Feed.h"

class Topic
{
public:
  Topic()
  : timestamp(0)
  {}

  Topic(const QByteArray &channel, const QByteArray &author, const QString &topic, qint64 timestamp)
  : author(author)
  , channel(channel)
  , timestamp(timestamp)
  , topic(topic)
  {}

  QByteArray author;
  QByteArray channel;
  qint64 timestamp;
  QString topic;
};


class SCHAT_EXPORT Channel
{
public:
  /// Ограничения.
  enum Limits {
    MinNameLengh = 3,     ///< Минимальная длина имени.
    MaxNameLength = 21,   ///< Максимальная длина имени.
    MaxDescLength = 128,  ///< Максимальная длина описания.
    MaxTopicLength = 1000 ///< Максимальная длина темы канала.
  };

  Channel();
  Channel(const QByteArray &id, const QString &name);
  ~Channel();
  bool addUser(const QByteArray &id);
  bool removeUser(const QByteArray &id);
  bool setId(const QByteArray &id);
  bool setName(const QString &name);
  bool setTopic(const QString &topic);
  bool setTopic(const QString &topic, const QByteArray &authorId, qint64 timestamp = 0);
  bool setUsers(const QList<QByteArray> &users);
  inline bool isSynced() const { return m_synced; }
  inline bool isValid() const { return m_valid; }
  inline int userCount() const { return m_users.size(); }
  inline QByteArray id() const { return m_id; }
  inline const QList<QByteArray>& users() { return m_users; }
  inline const QString& name() const { return m_name; }
  inline QVariant data() const { return m_data; }
  inline void clear() { m_users.clear(); }
  inline void setData(const QVariantMap &data) { m_data = data; }
  inline void setSynced(bool synced) { m_synced = synced; }
  Topic topic() const;

  // feeds.
  inline const Feeds& feeds() const { return m_feeds; }
  inline Feeds& feeds() { return m_feeds; }

private:
  inline bool validate(bool valid) { if (valid) return true; else m_valid = false; return false; }

  bool m_synced;                   ///< true если канал синхронизирован.
  bool m_valid;                    ///< true все данные корректны. \deprecated Не использовать эту переменную.
  Feeds m_feeds;                   ///< Фиды.
  QByteArray m_id;                 ///< Идентификатор канала.
  QList<QByteArray> m_users;       ///< Список идентификаторов пользователей в канале.
  QString m_name;                  ///< Имя канала.

  QString m_topic;                 ///< Тема канала. \deprecated Перенести хранение топика в фид.
  QVariantMap m_data;              ///< JSON данные канала. \deprecated Перенести хранение данныех в фиды.
};

typedef QSharedPointer<Channel> ClientChannel;

#endif /* CHANNEL_H_ */
