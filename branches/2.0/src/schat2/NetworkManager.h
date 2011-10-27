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
class MessageAdapter;
class NetworkManager;
class SettingsLegacy;
class SimpleClient;

/*!
 * Используется для хранения информации о подключении.
 * Поддерживает запись и чтение информации из конфигурационного файла.
 */
class SCHAT_CORE_EXPORT NetworkItem
{
public:
  NetworkItem();
  NetworkItem(const QByteArray &id);
  bool isValid() const;
  inline QByteArray cookie() const { return m_cookie; }
  inline QByteArray id() const { return m_id; }
  inline QString account() const { return m_account; }
  inline QString name() const { return m_name; }
  inline QString url() const { return m_url; }
  inline void setAccount(const QString &account) { m_account = account; }
  static NetworkItem item();
  void read();
  void write();

  friend class NetworkManager;

private:
  QString auth();
  void setAuth(const QString &auth);

  QByteArray m_cookie;   ///< Cookie.
  QByteArray m_id;       ///< Идентификатор сервера.
  QByteArray m_userId;   ///< Идентификатор пользователя.
  QString m_account;     ///< Имя пользователя.
  QString m_name;        ///< Имя сервера.
  QString m_url;         ///< Адрес сервера.
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
  inline bool isItem(const QByteArray &id) const { return m_items.contains(id); }
  inline int count() const { return m_items.count(); }
  inline NetworkItem item() { return item(serverId()); }
  inline NetworkItem item(const QByteArray &id) const { return m_items.value(id); }
  inline QString root() const { return root(serverId()); }
  QByteArray serverId() const;
  QList<NetworkItem> items() const;
  static QString currentServerName();
  void removeItem(const QByteArray &id);

private slots:
  void clientStateChanged(int state);
  void loggedIn(const QString &name);

private:
  QString root(const QByteArray &id) const;
  QStringList networkList() const;
  void load();
  void write();

  ChatSettings *m_settings; ///< Основные настройки.
  FileLocations *m_locations;
  MessageAdapter *m_adapter;
  QHash<QByteArray, NetworkItem> m_items;
  SimpleClient *m_client;   ///< Указатель на клиент.
};

#endif /* NETWORKMANAGER_H_ */
