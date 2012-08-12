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

class NodeLog;
class ServerData;
class Settings;

class SCHAT_EXPORT Storage : public QObject
{
  Q_OBJECT

public:
  Storage(QObject *parent = 0);
  ~Storage();
  inline static bool anonymous()           { return m_self->m_anonymous; }
  inline static bool nickOverride()        { return m_self->m_nickOverride; }
  inline static QByteArray privateId()     { return m_self->m_privateId; }
  inline static QByteArray serverId()      { return m_self->m_id; }
  inline static QString authServer()       { return m_self->m_authServer; }
  inline static Settings *settings()       { return m_self->m_settings; }
  inline static Storage *i()               { return m_self; }
  static bool hasFeature(const QString &name);
  static QString etcPath();
  static QString serverName();
  static QString sharePath();
  static QString varPath();
  static QVariant value(const QString &key, const QVariant &defaultValue = QVariant());
  static void addFeature(const QString &name);
  static void setValue(const QString &key, const QVariant &value);

  int load();
  int start();

private:
  void setDefaultSslConf();
  void setMaxOpenFiles(int max);

  bool m_anonymous;              ///< \b true если разрешена анонимная авторизация.
  bool m_nickOverride;           ///< \b true если разрешено зарегистрированным пользователям во время внешней авторизации, занимать ники анонимных пользователей если те не в сети.
  NodeLog *m_log;                ///< Журнал.
  QByteArray m_id;               ///< Публичный идентификатор сервера.
  QByteArray m_privateId;        ///< Приватный идентификатор сервера.
  QString m_authServer;          ///< Адрес авторизационного сервера.
  ServerData *m_serverData;      ///< Информация о сервере.
  Settings *m_settings;          ///< Настройки сервера.
  static QStringList m_features; ///< Список дополнительных API.
  static Storage *m_self;        ///< Указатель на себя.
};

#endif /* STORAGE_H_ */
