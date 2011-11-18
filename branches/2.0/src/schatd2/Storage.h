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

#include "net/packets/accounts.h"
#include "net/SimpleID.h"
#include "ServerChannel.h"
#include "ServerUser.h"

class DataBase;
class FileLocations;
class NodeLog;
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
  inline DataBase *db() const { return m_db; }
  inline QList<QByteArray> slaves() const { return m_slaves; }
  inline static Storage *i() { return m_self; }
  inline void removeSlave(const QByteArray &id) { m_slaves.removeAll(id); }
  inline void setAllowSlaves(bool allow = true) { m_allowSlaves = allow; }
  int start();
  static qint64 timestamp();
  static void merge(QList<quint64> &out, const QList<quint64> sockets);
  void addSlave(const QByteArray &id);

  // user management.
  bool isSameSlave(const QByteArray &id1, const QByteArray &id2);
  bool remove(ChatUser user);
  bool removeUserFromChannel(const QByteArray &userId, const QByteArray &channelId);
  ChatUser user(const QByteArray &id, bool offline = false) const;
  ChatUser user(const QString &nick, bool normalize) const;
  inline QHash<QByteArray, ChatUser> users() const { return m_users; }
  LoginReply login(ChatUser user, const QString &name, const QByteArray &password);
  QByteArray makeUserId(int type, const QByteArray &userId) const;
  QList<QByteArray> users(const QByteArray &id);
  RegReply reg(ChatUser user, const QString &name, const QByteArray &password, const QVariant &data);
  void rename(ChatUser user);
  void store(ChatUser user);
  void update(ChatUser user);

  // channel management.
  bool add(ChatChannel channel);
  bool removeChannel(const QByteArray &id);
  ChatChannel channel(ChatUser user);
  ChatChannel channel(const QByteArray &id, int type = SimpleID::ChannelId);
  ChatChannel channel(const QString &name);
  ChatChannel channel(qint64 id);
  inline QHash<QByteArray, ChatChannel> channels() const { return m_channels; }
  QList<quint64> sockets(ChatChannel channel);
  QList<quint64> sockets(const QList<QByteArray> &ids);
  void update(ChatChannel channel);

  inline FileLocations *locations() const { return m_locations; }
  inline ServerData *serverData() { return m_serverData; }
  inline Settings *settings() { return m_settings; }
  QByteArray cookie() const;
  QString normalize(const QString &text) const;

private:
  QByteArray makeChannelId(const QString &name) const;
  void setDefaultSslConf();

  bool m_allowSlaves;                            ///< true если разрешено подключение вторичных серверов.
  DataBase *m_db;                                ///< База данных сервера.
  FileLocations *m_locations;                    ///< Схема размещения файлов.
  NodeLog *m_log;                                ///< Журнал.
  QHash<QByteArray, ChatChannel> m_channels;     ///< Таблица каналов.
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
