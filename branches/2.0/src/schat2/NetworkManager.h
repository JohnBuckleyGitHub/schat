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

#ifndef NETWORKMANAGER_H_
#define NETWORKMANAGER_H_

#include <QObject>
#include <QStringList>
#include <QHash>

#include "schat.h"

class ChatSettings;
class FileLocations;
class NetworkManager;
class SettingsLegacy;
class SimpleClient;

class SCHAT_CORE_EXPORT NetworkItem
{
public:
  NetworkItem();
  NetworkItem(const QByteArray &id);
  bool isValid() const;
  inline QByteArray id() const { return m_id; }
  inline QString auth() const { return m_auth; }
  inline QString name() const { return m_name; }
  inline QString url() const { return m_url; }

  friend class NetworkManager;

private:
  QByteArray m_id; ///< Идентификатор сервера.
  QString m_auth;  ///< Строка аутентификации.
  QString m_name;  ///< Имя сервера.
  QString m_url;   ///< Адрес сервера.
};


/*!
 * Менеджер подключений.
 */
class SCHAT_CORE_EXPORT NetworkManager : public QObject
{
  Q_OBJECT

public:
  NetworkManager(QObject *parent = 0);
  bool open();
  bool open(const QByteArray &id);
  bool open(const QString &url);
  inline int count() const { return m_items.count(); }
  inline NetworkItem item(const QByteArray &id) const { return m_items.value(id); }
  QList<NetworkItem> items() const;
  static QString currentServerName();
  void removeItem(const QByteArray &id);

private slots:
  void clientStateChanged(int state);

private:
  QString authKey() const;
  QString root(const QByteArray &id);
  void load();
  void write();

  ChatSettings *m_settings; ///< Основные настройки.
  FileLocations *m_locations;
  QHash<QByteArray, NetworkItem> m_items;
  SimpleClient *m_client;   ///< Указатель на клиент.
};

#endif /* NETWORKMANAGER_H_ */
