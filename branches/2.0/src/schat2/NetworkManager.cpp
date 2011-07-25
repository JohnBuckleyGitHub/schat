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
  SimpleClient *client = ChatCore::i()->client();

  m_auth = SimpleID::toBase64(client->userId());
  m_name = NetworkManager::currentServerName();
  m_url = client->url().toString();
}


NetworkItem::NetworkItem(const QByteArray &id, const Settings &settings)
  : m_id(id)
{
  m_auth = settings.value(Auth).toString();
  m_url = settings.value(Url).toString();
  m_name = settings.value(Name).toString();
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
  , m_client(ChatCore::i()->client())
{
  load();
  connect(m_client, SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int)));
}


bool NetworkManager::open()
{
  if (m_client->clientState() != SimpleClient::ClientOffline)
    return false;

  if (m_settings->value(ChatSettings::AutoConnect).toBool() == false)
    return false;

  if (m_networks.isEmpty())
    return false;

  if (m_client->user()->status() == User::OfflineStatus)
    return false;

  open(SimpleID::fromBase64(m_networks.at(0).toLatin1()));

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


NetworkItem NetworkManager::item(const QByteArray &id) const
{
  return m_items.value(id);
}


QList<NetworkItem> NetworkManager::items() const
{
  QList<NetworkItem> out;
  for (int i = 0; i < m_networks.size(); ++i) {
    QByteArray id = SimpleID::fromBase64(m_networks.at(i).toLatin1());
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
  m_networks.removeAll(SimpleID::toBase64(id));
  m_settings->setValue(ChatSettings::Networks, m_networks);
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


QString NetworkManager::root(const QByteArray &id)
{
  QString out = m_settings->locations()->path(FileLocations::ConfigPath) + "/.networks/" + SimpleID::toBase64(id);
  if (!QFile::exists(out))
    QDir().mkpath(out);

  return out;
}


/*!
 * Формирование таблицы серверов при запуске.
 *
 * Список серверов читается из настройки Networks, если сервер прошёл
 * проверку на валидность то он добавляется в таблицу \p m_items.
 */
void NetworkManager::load()
{
  m_networks = m_settings->value(ChatSettings::Networks).toStringList();
  if (m_networks.isEmpty())
    return;

  Settings settings("");
  settings.setDefault("Auth", "");
  settings.setDefault("Url", "");
  settings.setDefault("Name", "");

  QStringList invalid;

  // Чтение данных серверов.
  for (int i = 0; i < m_networks.size(); ++i) {
    QByteArray id = SimpleID::fromBase64(m_networks.at(i).toLatin1());
    if (SimpleID::typeOf(id) != SimpleID::ServerId)
      continue;

    settings.setGroup(id.toHex());
    settings.read();

    NetworkItem item(id, settings);
    if (!item.isValid()) {
      invalid.append(m_networks.at(i));
      continue;
    }

    m_items.insert(id, item);
  }

  // Удаление не валидных серверов.
  if (!invalid.isEmpty()) {
    for (int i = 0; i < invalid.size(); ++i) {
      m_networks.removeAll(invalid.at(i));
    }
    m_settings->setValue(ChatSettings::Networks, m_networks);
  }
}


/*!
 * Запись информации о новом сервере.
 */
void NetworkManager::write()
{
  ServerData *data = m_client->serverData();
  QByteArray id = data->id();
  QString base64Id = SimpleID::toBase64(id);

  m_networks.removeAll(base64Id);
  m_networks.prepend(base64Id);
  m_settings->setValue(ChatSettings::Networks, m_networks);

  QString hexId = id.toHex();
  NetworkItem item(id);
  m_items[id] = item;

  m_settings->setAutoDefault(true);
  m_settings->setValue(hexId + "/Auth", item.auth(), false);
  m_settings->setValue(hexId + "/Url", item.url(), false);
  m_settings->setValue(hexId + "/Name", item.name(), false);
  m_settings->setAutoDefault(false);

  m_settings->write();

  ChatCore::i()->startNotify(ChatCore::NetworkChangedNotice, id);
}
