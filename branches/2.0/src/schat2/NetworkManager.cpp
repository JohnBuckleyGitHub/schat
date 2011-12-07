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

#include "debugstream.h"

#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/SimpleClient.h"
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
  SimpleClient *client = ChatClient::io();
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
  QSettings settings(ChatCore::locations()->path(FileLocations::ConfigFile), QSettings::IniFormat);
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
  QSettings settings(ChatCore::locations()->path(FileLocations::ConfigFile), QSettings::IniFormat);
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
  , m_invalids(0)
{
  load();
  connect(ChatClient::io(), SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int)));
}


bool NetworkManager::isAutoConnect() const
{
  if (ChatClient::io()->clientState() != SimpleClient::ClientOffline)
    return false;

  if (m_invalids)
    return false;

  if (!ChatCore::settings()->value("AutoConnect").toBool())
    return false;

//  if (m_client->user()->status() == Status::Offline)
//    return false;

  if (m_networks.data.isEmpty())
    return false;

  return true;
}


bool NetworkManager::open()
{
  if (isAutoConnect())
    return open(m_networks.first());

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

  ChatClient::io()->setCookieAuth(ChatCore::settings()->value("CookieAuth").toBool());
  NetworkItem item = m_items.value(id);
  ChatClient::io()->setAccount(item.account(), item.password());

  return ChatClient::io()->openUrl(item.url(), item.cookie());
}


/*!
 * Открытие нового соединения, URL адрес сервера.
 *
 * \param url Адрес сервера.
 * \return true в случае успеха.
 */
bool NetworkManager::open(const QString &url)
{
  return ChatClient::io()->openUrl(url);
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
  if (ChatClient::io()->clientState() == SimpleClient::ClientOnline && m_selected == ChatClient::io()->serverId())
    return 1;

  if (ChatClient::io()->clientState() == SimpleClient::ClientConnecting && ChatClient::io()->url().toString() == item(m_selected).url())
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
  QByteArray id = ChatClient::io()->serverId();
//  if (id.isEmpty()) {
//    QStringList networks = networkList();
//    if (!networks.isEmpty())
//      return SimpleID::decode(networks.at(0).toLatin1());
//  }

  return id;
}


/*!
 * Получение списка идентификаторов сетей, сохранённых в настройках.
 */
QList<NetworkItem> NetworkManager::items() const
{
  QList<NetworkItem> out;

  for (int i = 0; i < m_networks.data.size(); ++i) {
    NetworkItem item = m_items.value(m_networks.data.at(i));
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

  m_networks.data.removeAll(id);
  m_networks.write();
  ChatCore::settings()->remove(SimpleID::encode(id));

  if (id == ChatClient::io()->serverId() && ChatClient::io()->clientState() != SimpleClient::ClientOffline)
    ChatClient::io()->leave();
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
  QString out = ChatCore::locations()->path(FileLocations::ConfigPath) + "/networks/" + SimpleID::encode(id);
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
  m_networks.read();

  m_tmpId = SimpleID::make("", SimpleID::ServerId);
  NetworkItem item(m_tmpId);
  item.setUrl("schat://");
  m_items[m_tmpId] = item;

  if (m_networks.data.isEmpty())
    return;

  QList<QByteArray> invalids;
  Settings settings(ChatCore::locations()->path(FileLocations::ConfigFile), this);

  // Чтение данных серверов.
  for (int i = 0; i < m_networks.data.size(); ++i) {
    QByteArray id = m_networks.data.at(i);

    NetworkItem item(id);
    item.read();

    if (!item.isValid()) {
      invalids += id;
      continue;
    }

    m_items[id] = item;
  }

  // Удаление невалидных серверов.
  if (!invalids.isEmpty()) {
    for (int i = 0; i < invalids.size(); ++i) {
      m_networks.data.removeAll(invalids.at(i));
    }

    m_invalids = invalids.size();
    m_networks.write();
  }

  setSelected(m_networks.first());
}


/*!
 * Запись информации о новом сервере.
 */
void NetworkManager::write()
{
  QByteArray id = ChatClient::serverId();
  m_selected = id;

  NetworkItem item = NetworkItem::item();
  item.write();

  m_networks.data.removeAll(id);
  m_networks.data.prepend(id);
  m_networks.write();

  m_items[id] = item;
  root(id);
  ChatCore::i()->startNotify(ChatCore::NetworkChangedNotice, id);
}


NetworkManager::Networks::Networks()
{
}


/*!
 * Получение идентификатора первого сервера в списке или пустого
 * идентификатора если список пуст.
 */
QByteArray NetworkManager::Networks::first()
{
  if (data.isEmpty())
    return QByteArray();

  return data.at(0);
}


/*!
 * Загрузка списка сетей из настроек.
 */
void NetworkManager::Networks::read()
{
  data.clear();
  QStringList networks = ChatCore::settings()->value("Networks").toStringList();

  foreach (QString network, networks) {
    QByteArray id = SimpleID::decode(network.toLatin1());
    if (SimpleID::typeOf(id) == SimpleID::ServerId)
      data += id;
  }
}


/*!
 * Запись списка сетей в настройки.
 */
void NetworkManager::Networks::write()
{
  QStringList out;
  foreach (QByteArray id, data) {
    out += SimpleID::encode(id);
  }

  ChatCore::settings()->setValue("Networks", out);
}
