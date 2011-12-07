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
#include "messages/MessageAdapter.h"
#include "net/ServerData.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "User.h"

NetworkItem::NetworkItem()
  : m_authorized(false)
{
}


NetworkItem::NetworkItem(const QByteArray &id)
  : m_authorized(false)
  , m_id(id)
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
  item.m_userId = client->channelId();
//  item.m_account = client->user()->account();

  if (!item.m_account.isEmpty())
    item.m_authorized = true;

  return item;
}


void NetworkItem::read()
{
  QSettings settings(ChatCore::i()->locations()->path(FileLocations::ConfigFile), QSettings::IniFormat);
  settings.setIniCodec("UTF-8");

  settings.beginGroup(SimpleID::encode(m_id));
  m_account = settings.value("Account").toString();
  setAuth(settings.value("Auth").toString());
  m_name = settings.value("Name").toString();
  m_url  = settings.value("Url").toString();
  settings.endGroup();

  if (!m_account.isEmpty())
    m_authorized = true;
}


void NetworkItem::write()
{
  QSettings settings(ChatCore::i()->locations()->path(FileLocations::ConfigFile), QSettings::IniFormat);
  settings.setIniCodec("UTF-8");

  settings.beginGroup(SimpleID::encode(m_id));

  if (!m_account.isEmpty()) {
    settings.setValue("Account", m_account);
    m_authorized = true;
  }
  else
    settings.remove("Account");

  settings.setValue("Auth",    auth());
  settings.setValue("Name",    m_name);
  settings.setValue("Url",     m_url);
  settings.endGroup();
}


QString NetworkItem::auth()
{
  if (!isValid())
    return QString();

  QByteArray auth = m_id + m_userId + m_cookie;
  return SimpleID::toBase32(auth);
}


void NetworkItem::setAuth(const QString &auth)
{
  QByteArray data = SimpleID::fromBase32(auth.toLatin1());
  if (data.size() != SimpleID::DefaultSize * 3)
    return;

  m_userId = data.mid(SimpleID::DefaultSize, SimpleID::DefaultSize);
  m_cookie = data.mid(SimpleID::DefaultSize * 2, SimpleID::DefaultSize);
}


NetworkManager::NetworkManager(QObject *parent)
  : QObject(parent)
  , m_settings(ChatCore::i()->settings())
  , m_locations(ChatCore::i()->locations())
  , m_invalids(0)
  , m_client(ChatCore::i()->client())
{
  load();
  connect(m_client, SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int)));
}


bool NetworkManager::isAutoConnect() const
{
  if (m_client->clientState() != SimpleClient::ClientOffline)
    return false;

  if (m_invalids)
    return false;

  if (!m_settings->value(QLatin1String("AutoConnect")).toBool())
    return false;

//  if (m_client->user()->status() == Status::Offline)
//    return false;

  QStringList networks = networkList();
  if (networks.isEmpty())
    return false;

  return true;
}


bool NetworkManager::open()
{
  if (isAutoConnect())
    return open(SimpleID::decode(networkList().at(0).toLatin1()));

  return false;
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

  m_client->setCookieAuth(m_settings->value(QLatin1String("CookieAuth")).toBool());
  NetworkItem item = m_items.value(id);
  m_client->setAccount(item.account(), item.password());

  return m_client->openUrl(item.url(), item.cookie());
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
 * Возвращает состояние текущего выбранного итема.
 *
 * \return Возвращаемые значения:
 * - 0 Подключение не ассоциировано с выбранным итемом.
 * - 1 Подключение активно для текущего итема.
 * - 2 Идёт подключение.
 */
int NetworkManager::isSelectedActive() const
{
  if (m_client->clientState() == SimpleClient::ClientOnline && m_selected == m_client->serverId())
    return 1;

  if (m_client->clientState() == SimpleClient::ClientConnecting && m_client->url().toString() == item(m_selected).url())
    return 2;

  return 0;
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
      return SimpleID::decode(networks.at(0).toLatin1());
  }

  return id;
}


/*!
 * Получение списка идентификаторов сетей, сохранённых в настройках.
 */
QList<NetworkItem> NetworkManager::items() const
{
  QStringList networks = networkList();
  QList<NetworkItem> out;

  for (int i = 0; i < networks.size(); ++i) {
    NetworkItem item = m_items.value(SimpleID::decode(networks.at(i).toLatin1()));
    if (item.isValid())
      out.append(item);
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
 */
void NetworkManager::removeItem(const QByteArray &id)
{
  if (id == m_tmpId)
    return;

  QStringList networks = networkList();

  networks.removeAll(SimpleID::encode(id));
  m_settings->setValue(QLatin1String("Networks"), networks);
  m_settings->remove(SimpleID::encode(id));

  if (id == m_client->serverId() && m_client->clientState() != SimpleClient::ClientOffline)
    m_client->leave();
}


void NetworkManager::setSelected(const QByteArray &id)
{
  if (m_selected == id)
    return;

  m_selected = id;
  ChatCore::i()->startNotify(ChatCore::NetworkSelectedNotice, id);
}


void NetworkManager::clientStateChanged(int state)
{
  if (state != SimpleClient::ClientOnline)
    return;

  write();
}


QString NetworkManager::root(const QByteArray &id) const
{
  QString out = m_locations->path(FileLocations::ConfigPath) + QLatin1String("/networks/") + SimpleID::encode(id);
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
  m_tmpId = SimpleID::make("", SimpleID::ServerId);
  NetworkItem item(m_tmpId);
  item.setUrl("schat://");
  m_items[m_tmpId] = item;

  QStringList networks = networkList();
  if (networks.isEmpty())
    return;

  QStringList invalid;
  Settings settings(m_locations->path(FileLocations::ConfigFile), this);

  // Чтение данных серверов.
  for (int i = 0; i < networks.size(); ++i) {
    QByteArray id = SimpleID::decode(networks.at(i).toLatin1());
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

    m_items[id] = item;
  }

  // Удаление невалидных серверов.
  if (!invalid.isEmpty()) {
    for (int i = 0; i < invalid.size(); ++i) {
      networks.removeAll(invalid.at(i));
    }

    m_invalids = invalid.size();
    m_settings->setValue(QLatin1String("Networks"), networks);
  }

  if (!networks.isEmpty())
    setSelected(SimpleID::decode(networks.at(0).toLatin1()));
}


/*!
 * Запись информации о новом сервере.
 */
void NetworkManager::write()
{
  QByteArray id = m_client->serverData()->id();
  m_selected = id;
  QString base64 = SimpleID::encode(id);

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
