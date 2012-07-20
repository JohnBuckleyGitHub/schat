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

#include <QFile>
#include <QDir>

#include "Account.h"
#include "ChatCore.h"
#include "ChatHooks.h"
#include "ChatNotify.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/SimpleClient.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "Path.h"
#include "sglobal.h"

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


NetworkItem* NetworkItem::item()
{
  SimpleClient *client = ChatClient::io();
  NetworkItem *item = new NetworkItem(ChatClient::serverId());

  item->m_name    = ChatClient::serverName();
  item->m_url     = client->url().toString();
  item->m_cookie  = client->cookie();
  item->m_userId  = client->channelId();

  return item;
}


void NetworkItem::read()
{
  QSettings settings(Path::config(), QSettings::IniFormat);
  settings.setIniCodec("UTF-8");

  settings.beginGroup(SimpleID::encode(m_id));
  setAuth(settings.value(LS("Auth")).toString());
  m_name = settings.value(LS("Name")).toString();
  m_url  = settings.value(LS("Url")).toString();
  settings.endGroup();
}


void NetworkItem::write()
{
  QSettings settings(Path::config(), QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup(SimpleID::encode(m_id));
  settings.setValue(LS("Auth"),    auth());
  settings.setValue(LS("Name"),    m_name);
  settings.setValue(LS("Url"),     m_url);
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

  new Hooks::Networks(this);

  connect(ChatClient::io(), SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int)));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
}


bool NetworkManager::isAutoConnect() const
{
  if (ChatClient::state() != ChatClient::Offline)
    return false;

  if (m_invalids)
    return false;

  if (!ChatCore::settings()->value(LS("AutoConnect")).toBool())
    return false;

  if (ChatClient::channel()->status().value() == Status::Offline)
    return false;

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

  ChatClient::io()->setCookieAuth(ChatCore::settings()->value(LS("Labs/CookieAuth")).toBool());
  Network item = m_items.value(id);
//  ChatClient::io()->setAccount(item->account(), item->password());

  return ChatClient::io()->openUrl(item->url(), item->cookie());
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
  if (ChatClient::state() == ChatClient::Online && m_selected == ChatClient::serverId())
    return 1;

  if (ChatClient::state() == ChatClient::Connecting && ChatClient::io()->url().toString() == item(m_selected)->url())
    return 2;

  return 0;
}


Network NetworkManager::item(const QByteArray &id) const
{
  if (!m_items.contains(id))
    return m_items.value(m_tmpId);

  return m_items.value(id);
}


/*!
 * Получение списка идентификаторов сетей, сохранённых в настройках.
 */
QList<Network> NetworkManager::items() const
{
  QList<Network> out;

  for (int i = 0; i < m_networks.data.size(); ++i) {
    Network item = m_items.value(m_networks.data.at(i));
    if (item->isValid())
      out.append(item);
  }

  return out;
}


/*!
 * Возвращает и при необходимости создаёт путь для хранения файлов сервера.
 */
QString NetworkManager::root(const QByteArray &id) const
{
  if (id.isEmpty())
    return QString();

  QString out = Path::cache() + LC('/') + SimpleID::encode(id);
  if (!QFile::exists(out))
    QDir().mkpath(out);

  return out;
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

  if (id == ChatClient::serverId() && ChatClient::state() != ChatClient::Offline)
    ChatClient::io()->leave();
}


void NetworkManager::setSelected(const QByteArray &id)
{
  if (m_selected == id)
    return;

  m_selected = id;
  ChatNotify::start(Notify::NetworkSelected, id);
}


/*!
 * Возвращает \b true если требуется принудительная авторизация по имени и паролю.
 */
bool NetworkManager::isPasswordRequired()
{
  if (ChatClient::state() != ChatClient::Error)
    return false;

  QVariantMap error = ChatClient::io()->json().value(LS("error")).toMap();
  if (error.isEmpty())
    return false;

  if (error.value("status") != Notice::Unauthorized)
    return false;

  if (ChatCore::networks()->selected() != SimpleID::decode(ChatClient::io()->json().value(LS("id")).toByteArray()))
    return false;

  return true;
}


void NetworkManager::clientStateChanged(int state)
{
  if (state != ChatClient::Online)
    return;

  write();
}


void NetworkManager::notify(const Notify &notify)
{
  if (notify.type() == Notify::ServerRenamed) {
    Network item = this->item(ChatClient::serverId());
    if (!item)
      return;

    item->setName(ChatClient::serverName());
    item->write();
  }
  else if (notify.type() == Notify::FeedReply) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (n.match(ChatClient::id(), LS("account"), LS("login")) || n.match(ChatClient::id(), LS("account"), LS("reset")))
      login();
  }
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
  Network item(new NetworkItem(m_tmpId));
  item->setUrl(LS("schat://"));
  m_items[m_tmpId] = item;

  if (m_networks.data.isEmpty())
    return;

  QList<QByteArray> invalids;
  Settings settings(Path::config(), this);

  // Чтение данных серверов.
  for (int i = 0; i < m_networks.data.size(); ++i) {
    QByteArray id = m_networks.data.at(i);

    Network item(new NetworkItem(id));
    item->read();

    if (!item->isValid()) {
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
 * \deprecated Эта функция больше не нужна.
 */
void NetworkManager::login()
{
  ChatClient::io()->leave();
//  ChatClient::i()->login();
}


/*!
 * Запись информации о новом сервере.
 */
void NetworkManager::write()
{
  QByteArray id = ChatClient::serverId();
  m_selected = id;

  Network item(NetworkItem::item());
  item->write();

  m_networks.data.removeAll(id);
  m_networks.data.prepend(id);
  m_networks.write();

  m_items[id] = item;
  root(id);
  ChatNotify::start(Notify::NetworkChanged, id);
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
  QStringList networks = ChatCore::settings()->value(LS("Networks")).toStringList();

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

  ChatCore::settings()->setValue(LS("Networks"), out);
}
