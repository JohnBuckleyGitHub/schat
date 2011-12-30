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

#ifndef STORAGE_H_
#define STORAGE_H_

#include <QByteArray>
#include <QHash>

#include "net/SimpleID.h"
#include "ServerChannel.h"

class DataBase;
class FileLocations;
class NodeLog;
class ServerData;
class Settings;
class StorageHooks;

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
  inline StorageHooks *hooks() const { return m_hooks; }
  inline void removeSlave(const QByteArray &id) { m_slaves.removeAll(id); }
  inline void setAllowSlaves(bool allow = true) { m_allowSlaves = allow; }
  int start();
  void addSlave(const QByteArray &id);

  // user management.
  QByteArray makeUserId(int type, const QByteArray &userId) const;

  // channel management.
  bool add(ChatChannel channel);
  bool gc(ChatChannel channel);
  ChatChannel channel(const QByteArray &id, int type = SimpleID::ChannelId);
  ChatChannel channel(const QString &name);
  void remove(ChatChannel channel);
  void rename(ChatChannel channel, const QString &name);
  void update(ChatChannel channel);

  inline FileLocations *locations() const { return m_locations; }
  inline static ServerData *serverData() { return i()->m_serverData; }
  inline static Settings *settings() { return i()->m_settings; }
  QByteArray cookie() const;

private:
  QByteArray makeId(const QByteArray &normalized) const;
  void setDefaultSslConf();

  /// Внутренний кэш хранилища.
  class Cache
  {
  public:
    Cache() {}
    inline ChatChannel channel(const QByteArray &id) const { return m_channels.value(id); }
    void add(ChatChannel channel);
    void remove(const QByteArray &id);
    void rename(ChatChannel channel, const QByteArray &before);

  private:
    QHash<QByteArray, ChatChannel> m_channels;
  };

  bool m_allowSlaves;                            ///< true если разрешено подключение вторичных серверов.
  Cache m_cache;                                 ///< Кеш хранилища.
  DataBase *m_db;                                ///< База данных сервера.
  FileLocations *m_locations;                    ///< Схема размещения файлов.
  NodeLog *m_log;                                ///< Журнал.
  QList<QByteArray> m_slaves;                    ///< Список вторичных серверов.
  ServerData *m_serverData;                      ///< Информация о сервере.
  Settings *m_settings;                          ///< Настройки сервера.
  static Storage *m_self;                        ///< Указатель на себя.
  StorageHooks *m_hooks;                         ///< Обработчик хуков.
};

#endif /* STORAGE_H_ */
