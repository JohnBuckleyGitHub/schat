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

#include "ServerChannel.h"
#include "ServerUser.h"

class DataBase;
class FileLocations;
class ServerData;
class Settings;

class SCHAT_EXPORT Storage : public QObject
{
  Q_OBJECT

public:
  Storage(QObject *parent = 0);
  ~Storage();
  inline bool isAllowSlaves() const { return m_allowSlaves; }
  inline bool isSlave(const QByteArray &id) { return m_slaves.contains(id); }
  inline QList<QByteArray> slaves() const { return m_slaves; }
  inline static Storage *i() { return m_self; }
  inline void setAllowSlaves(bool allow = true) { m_allowSlaves = allow; }
  int start();
  void addSlave(const QByteArray &id);

  // user management.
  bool add(ChatUser user);
  bool isSameSlave(const QByteArray &id1, const QByteArray &id2);
  bool remove(ChatUser user);
  bool removeUserFromChannel(const QByteArray &userId, const QByteArray &channelId);
  ChatUser user(const QString &nick, bool normalize) const;
  inline ChatUser user(const QByteArray &id) const { return m_users.value(id); }
  inline QHash<QByteArray, ChatUser> users() const { return m_users; }
  QByteArray makeUserId(int type, const QByteArray &clientId) const;
  void rename(ChatUser user);

  // channel management.
  bool removeChannel(const QByteArray &id);
  ChatChannel addChannel(ChatUser user);
  ChatChannel addChannel(const QString &name, bool permanent = false);
  ChatChannel channel(const QString &name, bool normalize) const;
  inline ChatChannel channel(const QByteArray &id) const { return m_channels.value(id); }
  inline QHash<QByteArray, ChatChannel> channels() const { return m_channels; }
  QList<quint64> socketsFromChannel(ChatChannel channel);
  QList<quint64> socketsFromIds(const QList<QByteArray> &ids);
  void addChannel(ChatChannel channel);

  inline FileLocations *locations() const { return m_locations; }
  inline ServerData *serverData() { return m_serverData; }
  inline Settings *settings() { return m_settings; }
  QByteArray session() const;
  QString normalize(const QString &text) const;

private:
  QByteArray makeChannelId(const QString &name);

  bool m_allowSlaves;                            ///< true если разрешено подключение вторичных серверов.
  DataBase *m_db;                                ///< База данных сервера.
  FileLocations *m_locations;                    ///< Схема размещения файлов.
  QHash<QByteArray, ChatChannel> m_channels;     ///< Таблица каналов.
  QHash<QByteArray, ChatUser> m_sessions;        ///< Таблица сессий.
  QHash<QByteArray, ChatUser> m_users;           ///< Таблица пользователей.
  QHash<QChar, QChar> m_normalize;               ///< Карта замены символов при нормализации ника.
  QHash<QString, ChatChannel> m_channelNames;    ///< Имена каналов.
  QHash<QString, ChatUser> m_nicks;              ///< Таблица ников.
  QList<QByteArray> m_slaves;                    ///< Список вторичных серверов.
  ServerData *m_serverData;                      ///< Информация о сервере.
  Settings *m_settings;                          ///< Настройки сервера.
  static Storage *m_self;                        ///< Указатель на себя.
};

#endif /* STORAGE_H_ */
