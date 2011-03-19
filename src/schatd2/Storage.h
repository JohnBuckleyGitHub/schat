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

#ifndef STORAGE_H_
#define STORAGE_H_

#include <QByteArray>
#include <QHash>

class ServerChannel;
class ServerUser;

class Storage
{
public:
  Storage(const QByteArray &id);
  ~Storage();

  // user management.
  bool add(ServerUser *user);
  bool remove(const QByteArray &id);
  bool removeUserFromChannel(const QByteArray &userId, const QByteArray &channelId);
  inline ServerUser* user(const QByteArray &id) const { return m_users.value(id); }
  QByteArray makeUserId(int type, const QByteArray &clientId) const;
  QList<quint64> socketsFromUser(ServerUser *user);
  ServerUser* user(const QString &nick, bool normalize) const;

  // channel management.
  bool removeChannel(const QByteArray &id);
  inline ServerChannel* channel(const QByteArray &id) const { return m_channels.value(id); }
  QList<quint64> socketsFromChannel(ServerChannel *channel);
  ServerChannel* addChannel(const QString &name, bool permanent = false);
  ServerChannel* channel(const QString &name, bool normalize) const;

  inline QByteArray id() const { return m_id; }
  QByteArray session() const;
  QString normalize(const QString &text) const;

private:
  QByteArray makeChannelId(const QString &name);

  QByteArray m_id;                               ///< Уникальный идентификатор сервера.
  QHash<QByteArray, ServerChannel*> m_channels;  ///< Таблица каналов.
  QHash<QByteArray, ServerUser*> m_sessions;     ///< Таблица сессий.
  QHash<QByteArray, ServerUser*> m_users;        ///< Таблица пользователей.
  QHash<QChar, QChar> m_normalize;               ///< Карта замены символов при нормализации ника.
  QHash<QString, ServerChannel*> m_channelNames; ///< Имена каналов.
  QHash<QString, ServerUser*> m_nicks;           ///< Таблица ников.
};

#endif /* STORAGE_H_ */
