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

#include <QFile>
#include <QDir>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/SimpleClient.h"
#include "debugstream.h"
#include "FileLocations.h"
#include "net/ServerData.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "User.h"

NetworkItem::NetworkItem()
{
}


NetworkItem::NetworkItem(const QByteArray &id)
  : m_id(id)
{
}


bool NetworkItem::isValid() const
{
  if (m_auth.isEmpty())
    return false;

  if (m_url.isEmpty())
    return false;

  if (m_name.isEmpty())
    return false;

  return true;
}


NetworkManager::NetworkManager(QObject *parent)
  : QObject(parent)
  , m_settings(ChatCore::i()->settings())
  , m_locations(ChatCore::i()->locations())
  , m_client(ChatCore::i()->client())
{
  load();
  connect(m_client, SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int)));
}


bool NetworkManager::open()
{
  if (m_client->clientState() != SimpleClient::ClientOffline)
    return false;

  if (m_settings->value(QLatin1String("AutoConnect")).toBool() == false)
    return false;

  if (m_client->user()->status() == User::OfflineStatus)
    return false;

  QStringList networks = networkList();
  if (networks.isEmpty())
    return false;

  open(SimpleID::fromBase64(networks.at(0).toLatin1()));

  return true;
}


/*!
 * Открытие нового соединения, используя идентификатор сервера.
 *
 * \param id Сохранённый идентификатор сервера.
 * \return true в случае успеха.
 */
bool NetworkManager::open(const QByteArray &id)
{
  if (!m_items.contains(id))
    return false;

  NetworkItem item = m_items.value(id);
  return m_client->openUrl(item.url());
}


/*!
 * Открытие нового соединения, URL адрес сервера.
 *
 * \param url Адрес сервера.
 * \return true в случае успеха.
 */
bool NetworkManager::open(const QString &url)
{
  return m_client->openUrl(url);
}


/*!
 * Получение идентификатора сервера.
 *
 * \return Идентификатор сервера или пустой массив, если клиент никогда не был подключен к серверу.
 */
QByteArray NetworkManager::serverId() const
{
  QByteArray id = m_client->serverId();
  if (id.isEmpty()) {
    QStringList networks = networkList();
    if (!networks.isEmpty())
      return SimpleID::fromBase64(networks.at(0).toLatin1());
  }

  return id;
}


QList<NetworkItem> NetworkManager::items() const
{
  QStringList networks = networkList();
  QList<NetworkItem> out;

  for (int i = 0; i < networks.size(); ++i) {
    QByteArray id = SimpleID::fromBase64(networks.at(i).toLatin1());
    if (SimpleID::typeOf(id) == SimpleID::ServerId)
      out.append(m_items.value(id));
  }

  return out;
}


/*!
 * Получение директории с рабочими файлами для текущего сервера.
 */
QString NetworkManager::root() const
{
  return root(serverId());
}


/*!
 * Возвращает имя текущего сервера.
 */
QString NetworkManager::currentServerName()
{
  QString name = ChatCore::i()->client()->serverData()->name();
  if (name.isEmpty())
    name = ChatCore::i()->client()->url().host();

  return name;
}


/*!
 * Удаление сервера.
 *
 * \todo ! Также реализовать полное удаление всех настроек этого сервера.
 */
void NetworkManager::removeItem(const QByteArray &id)
{
  QStringList networks = networkList();

  networks.removeAll(SimpleID::toBase64(id));
  m_settings->setValue(QLatin1String("Networks"), networks);
}


void NetworkManager::clientStateChanged(int state)
{
  if (state != SimpleClient::ClientOnline)
    return;

  write();
}


QString NetworkManager::authKey() const
{
  return SimpleID::toBase64(m_client->userId());
}


QString NetworkManager::root(const QByteArray &id) const
{
  QString out = m_locations->path(FileLocations::ConfigPath) + QLatin1String("/networks/") + SimpleID::toBase64(id);
  if (!QFile::exists(out))
    QDir().mkpath(out);

  return out;
}


QStringList NetworkManager::networkList() const
{
  return m_settings->value(QLatin1String("Networks")).toStringList();
}


/*!
 * Формирование таблицы серверов при запуске.
 *
 * Список серверов читается из настройки Networks, если сервер прошёл
 * проверку на валидность то он добавляется в таблицу \p m_items.
 */
void NetworkManager::load()
{
  QStringList networks = networkList();
  if (networks.isEmpty())
    return;

  QStringList invalid;
  Settings settings(m_locations->path(FileLocations::ConfigFile), this);

  // Чтение данных серверов.
  for (int i = 0; i < networks.size(); ++i) {
    QByteArray id = SimpleID::fromBase64(networks.at(i).toLatin1());
    if (SimpleID::typeOf(id) != SimpleID::ServerId) {
      invalid.append(networks.at(i));
      continue;
    }

    NetworkItem item(id);
    settings.beginGroup(id.toHex());
    item.m_auth = settings.value(QLatin1String("Auth"), item.auth()).toString();
    item.m_url  = settings.value(QLatin1String("Url"),  item.url()).toString();
    item.m_name = settings.value(QLatin1String("Name"), item.name()).toString();
    settings.endGroup();

    if (!item.isValid()) {
      invalid.append(networks.at(i));
      continue;
    }

    m_items.insert(id, item);
  }

  // Удаление невалидных серверов.
  if (!invalid.isEmpty()) {
    for (int i = 0; i < invalid.size(); ++i) {
      networks.removeAll(invalid.at(i));
    }
    m_settings->setValue(QLatin1String("Networks"), networks);
  }
}


/*!
 * Запись информации о новом сервере.
 */
void NetworkManager::write()
{
  QByteArray id  = m_client->serverData()->id();
  QString base64 = SimpleID::toBase64(id);

  QStringList networks = networkList();
  networks.removeAll(base64);
  networks.prepend(base64);
  m_settings->setValue(QLatin1String("Networks"), networks);

  NetworkItem item = m_items.value(id);
  item.m_id   = id;
  item.m_auth = SimpleID::toBase64(m_client->userId());
  item.m_name = currentServerName();
  item.m_url  = m_client->url().toString();

  Settings settings(m_locations->path(FileLocations::ConfigFile), this);
  settings.beginGroup(id.toHex());
  settings.setValue(QLatin1String("Auth"), item.auth());
  settings.setValue(QLatin1String("Url"),  item.url());
  settings.setValue(QLatin1String("Name"), item.name());
  settings.endGroup();

  m_items[id] = item;
  root(id);
  ChatCore::i()->startNotify(ChatCore::NetworkChangedNotice, id);
}
