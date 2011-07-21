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
#include <QString>

class Channel
{
public:
  /// Ограничения.
  enum Limits {
    MinNameLengh = 3,     ///< Минимальная длина имени.
    MaxNameLength = 20,   ///< Максимальная длина имени.
    MaxDescLength = 128,  ///< Максимальная длина описания.
    MaxTopicLength = 1024 ///< Максимальная длина темы канала.
  };

  enum ChannelType {
    GenericChannel = 0x67,
    PrivateChannel = 0x70
  };

  Channel();
  Channel(const QByteArray &id, const QString &name);
  ~Channel();
  bool addUser(const QByteArray &id);
  bool removeUser(const QByteArray &id);
  bool setDesc(const QString &desc);
  bool setId(const QByteArray &id);
  bool setName(const QString &name);
  bool setTopic(const QString &topic);
  bool setUsers(const QList<QByteArray> &users);
  inline bool isSynced() const { return m_synced; }
  inline bool isValid() const { return m_valid; }
  inline int userCount() const { return m_users.size(); }
  inline QByteArray id() const { return m_id; }
  inline QList<QByteArray> users() { return m_users; }
  inline QString desc() const { return m_desc; }
  inline QString name() const { return m_name; }
  inline QString topic() const { return m_topic; }
  inline void clear() { m_users.clear(); }
  inline void setSynced(bool synced) { m_synced = synced; }

private:
  inline bool validate(bool valid) { if (valid) return true; else m_valid = false; return false; }

  bool m_synced;             ///< true если канал синхронизирован.
  bool m_valid;              ///< true все данные корректны.
  ChannelType m_type;        ///< Тип канала.
  QByteArray m_id;           ///< Идентификатор канала.
  QList<QByteArray> m_users; ///< Список идентификаторов пользователей в канале.
  QString m_desc;            ///< Описание канала.
  QString m_name;            ///< Имя канала.
  QString m_topic;           ///< Тема канала.
};

#endif /* CHANNEL_H_ */
