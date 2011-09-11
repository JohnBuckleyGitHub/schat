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
  if (m_id.isEmpty() || m_userId.isEmpty() || m_cookie.isEmpty() || m_url.isEmpty() || m_name.isEmpty())
    return false;

  if (SimpleID::typeOf(m_id) != SimpleID::ServerId)
    return false;

  if (SimpleID::typeOf(m_cookie) != SimpleID::CookieId)
    return false;

  if (SimpleID::typeOf(m_userId) != SimpleID::UserId)
    return false;

  return true;
}


/*!
 *
 */
NetworkItem NetworkItem::item()
{
  SimpleClient *client = ChatCore::i()->client();
  NetworkItem item(client->serverData()->id());

  QString name = client->serverData()->name();
  if (name.isEmpty())
    name = client->url().host();

  item.m_name = name;
  item.m_url = client->url().toString();
  item.m_cookie = client->cookie();
  item.m_userId = client->userId();

  return item;
}


void NetworkItem::read()
{
  QSettings settings(ChatCore::i()->locations()->path(FileLocations::ConfigFile), QSettings::IniFormat);
  settings.setIniCodec("UTF-8");

  settings.beginGroup(m_id.toHex());
  setAuth(settings.value(QLatin1String("Auth")).toString());
  m_url  = settings.value(QLatin1String("Url")).toString();
  m_name = settings.value(QLatin1String("Name")).toString();
  settings.endGroup();
}


void NetworkItem::write()
{
  QSettings settings(ChatCore::i()->locations()->path(FileLocations::ConfigFile), QSettings::IniFormat);
  settings.setIniCodec("UTF-8");

  settings.beginGroup(m_id.toHex());
  settings.setValue(QLatin1String("Auth"), auth());
  settings.setValue(QLatin1String("Url"),  m_url);
  settings.setValue(QLatin1String("Name"), m_name);
  settings.endGroup();
}


QString NetworkItem::auth()
{
  if (!isValid())
    return QString();

  QByteArray auth = m_id + m_userId + m_cookie;
  return SimpleID::toBase64(auth);
}


void NetworkItem::setAuth(const QString &auth)
{
  QByteArray data = SimpleID::fromBase64(auth.toLatin1());
  if (data.size() != SimpleID::DefaultSize * 3)
    return;

  m_userId = data.mid(SimpleID::DefaultSize, SimpleID::DefaultSize);
  m_cookie = data.mid(SimpleID::DefaultSize * 2, SimpleID::DefaultSize);
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
 * Возвращает имя текущего сервера.
 */
QString NetworkManager::currentServerName()
{
  return ChatCore::i()->networks()->item().name();
}


/*!
 * Удаление сервера.
 */
void NetworkManager::removeItem(const QByteArray &id)
{
  QStringList networks = networkList();

  networks.removeAll(SimpleID::toBase64(id));
  m_settings->setValue(QLatin1String("Networks"), networks);
  m_settings->remove(id.toHex());
}


void NetworkManager::clientStateChanged(int state)
{
  if (state != SimpleClient::ClientOnline)
    return;

  write();
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
    item.read();

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

  NetworkItem item = NetworkItem::item();
  item.write();

  m_items[id] = item;
  root(id);
  ChatCore::i()->startNotify(ChatCore::NetworkChangedNotice, id);
}
