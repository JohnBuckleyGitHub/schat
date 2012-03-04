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

#include <QDebug>

#include "ChatNotify.h"
#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientFeeds.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "WebBridge.h"

WebBridge *WebBridge::m_self = 0;

WebBridge::WebBridge(QObject *parent)
  : QObject(parent)
{
  m_self = this;
}


QString WebBridge::channel(const QString &id)
{
  return JSON::generate(channel(SimpleID::decode(id.toLatin1())));
}


/*!
 * Получение фида по идентификатору канала и имени.
 */
QString WebBridge::feed(const QString &id, const QString &name, bool cache)
{
  ClientChannel channel = ChatClient::channels()->get(SimpleID::decode(id));
  if (!channel)
    return QString();

  return JSON::generate(feed(channel, name, cache));
}


/*!
 * Получение собственного фида по имени.
 */
QString WebBridge::feed(const QString &name, bool cache)
{
  return JSON::generate(feed(ChatClient::channel(), name, cache));
}


void WebBridge::setTabPage(const QString &id, int page)
{
  ClientChannel channel = ChatClient::channels()->get(SimpleID::decode(id.toLatin1()));
  if (!channel)
    return;

  channel->data()[LS("page")] = page;
}


QVariantMap WebBridge::channel(const QByteArray &id)
{
  ClientChannel channel = ChatClient::channels()->get(id);
  if (!channel)
    return QVariantMap();

  QVariantMap data;
  data[LS("Id")]   = SimpleID::encode(id);
  data[LS("Name")] = channel->name();
  data[LS("Url")]  = ChatUrls::toUrl(channel, LS("insert")).toString();
  return data;
}


/*!
 * Базовая функция получения фида.
 *
 * \param channel Канал владелец фида.
 * \param name    Имя фида.
 * \param cache   \b false если необходимо игнорировать локальный кэш и получить фид с сервера.
 *
 * \return JSON тело фида или пустую таблицу, если фид не доступен локально.
 */
QVariantMap WebBridge::feed(ClientChannel channel, const QString &name, bool cache)
{
  if (!cache)
    ChatClient::feeds()->request(channel->id(), LS("get"), name);

  FeedPtr feed = channel->feed(name, false);
  if (!feed)
    return QVariantMap();

  return feed->data();
}


QVariantMap WebBridge::feed(const FeedNotify &notify)
{
  QVariantMap out;
  ClientChannel channel = ChatClient::channels()->get(notify.channel());
  if (!channel)
    return out;

  FeedPtr feed = channel->feed(notify.name(), false);
  if (!feed)
    return out;

  out[LS("name")] = notify.name();
  out[LS("own")]  = notify.channel() == ChatClient::id();
  out[LS("feed")] = feed->data();

  return out;
}
