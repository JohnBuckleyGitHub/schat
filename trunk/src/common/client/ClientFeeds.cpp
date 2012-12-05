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

#include "client/ChatClient.h"
#include "client/ClientFeeds.h"
#include "client/ClientHooks.h"
#include "client/SimpleClient.h"
#include "net/packets/FeedNotice.h"
#include "net/packets/Notice.h"
#include "sglobal.h"

ClientFeeds::ClientFeeds(QObject *parent)
  : QObject(parent)
{
  m_hooks = new Hooks::Feeds(this);

  connect(ChatClient::io(), SIGNAL(notice(int)), SLOT(notice(int)));
}


bool ClientFeeds::del(const QByteArray &id, const QString &name, int options)
{
  QVariantMap json;
  json[LS("options")] = options;
  return request(id, LS("delete"), name, json);
}


bool ClientFeeds::post(const QByteArray &id, const QString &name, const QVariant &value, int options)
{
  QVariantMap json;
  json[LS("value")]   = value;
  json[LS("options")] = options;
  return request(id, LS("post"), name, json);
}


bool ClientFeeds::put(const QByteArray &id, const QString &name, const QVariant &value, int options)
{
  QVariantMap json;
  json[LS("value")]   = value;
  json[LS("options")] = options;
  return request(id, LS("put"), name, json);
}


/*!
 * Запрос к фиду.
 *
 * Если фид не изменился, сервер вернут статус "303 Not Modified".
 *
 * \param channel Канал, к которому предназначен запрос.
 * \param command Команда отправленная фиду.
 * \param name    Имя фида с опциональным путём запроса.
 * \param json    Опциональные JSON данные.
 */
bool ClientFeeds::request(ClientChannel channel, const QString &command, const QString &name, const QVariantMap &json)
{
  if (!channel || command.isEmpty() || name.isEmpty())
    return false;

  FeedPacket packet = FeedNotice::request(ChatClient::id(), channel->id(), command, name, json);
  if (FeedNotice::split(name).second.isEmpty()) {
    FeedPtr feed = channel->feed(name, false);
    if (feed)
      packet->setDate(feed->head().date());
  }

  return ChatClient::io()->send(packet);
}


/*!
 * Запрос к фиду.
 *
 * \param id      Идентификатор канала, к которому предназначен запрос.
 * \param command Команда отправленная фиду.
 * \param name    Имя фида с опциональным путём запроса.
 * \param json    Опциональные JSON данные.
 */
bool ClientFeeds::request(const QByteArray &id, const QString &command, const QString &name, const QVariantMap &json)
{
  if (!Channel::isCompatibleId(id) || command.isEmpty() || name.isEmpty())
    return false;

  return ChatClient::io()->send(FeedNotice::request(ChatClient::id(), id, command, name, json));
}


/*!
 * Проверка прав доступа.
 *
 * \return -1 если произошла ошибка или права доступа.
 */
int ClientFeeds::match(ClientChannel channel, ClientChannel user)
{
  FeedPtr feed = channel->feed(LS("acl"), false);
  if (!feed)
    return -1;

  return feed->head().acl().match(user.data());
}


void ClientFeeds::notice(int type)
{
  if (type != Notice::FeedType)
    return;

  FeedNotice packet(type, ChatClient::io()->reader());
  if (!packet.isValid())
    return;

  m_packet = &packet;
  m_hooks->readFeed(packet);
}
