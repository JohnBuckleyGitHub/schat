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

ClientFeeds::ClientFeeds(QObject *parent)
  : QObject(parent)
{
  m_hooks = new Hooks::Feeds(this);

  connect(ChatClient::io(), SIGNAL(notice(int)), SLOT(notice(int)));
}


bool ClientFeeds::add(const QByteArray &id, const QString &name, const QVariantMap &json)
{
  return request(id, "add", name, json);
}


bool ClientFeeds::clear(const QByteArray &id, const QString &name)
{
  return request(id, "clear", name);
}


bool ClientFeeds::get(const QByteArray &id, const QString &name)
{
  return request(id, "get", name);
}


/*!
 * Запрос заголовков фидов.
 *
 * \param id Идентификатор канала, к которому предназначен запрос.
 */
bool ClientFeeds::headers(const QByteArray &id)
{
  if (!Channel::isCompatibleId(id))
    return false;

  return ChatClient::io()->send(FeedNotice::headers(ChatClient::id(), id, ChatClient::io()->sendStream()));
}


bool ClientFeeds::query(const QByteArray &id, const QString &name, const QVariantMap &json)
{
  return request(id, "query", name, json);
}


bool ClientFeeds::remove(const QByteArray &id, const QString &name)
{
  return request(id, "remove", name);
}


bool ClientFeeds::request(const QByteArray &id, const QString &command, const QString &name, const QVariantMap &json)
{
  if (!Channel::isCompatibleId(id))
    return false;

  if (command.isEmpty())
    return false;

  if (name.isEmpty())
    return false;

  return ChatClient::io()->send(FeedNotice::request(ChatClient::id(), id, command, name, ChatClient::stream(), json));
}


bool ClientFeeds::revert(const QByteArray &id, const QString &name, qint64 rev)
{
  QVariantMap json;
  if (rev > 0)
    json["rev"] = rev;

  return request(id, "revert", name, json);
}


bool ClientFeeds::update(const QByteArray &id, const QString &name, const QVariantMap &json)
{
  return request(id, "update", name, json);
}


void ClientFeeds::notice(int type)
{
  if (type != Notice::FeedType)
    return;

  FeedNotice packet(type, ChatClient::io()->reader());
  if (!packet.isValid())
    return;

  m_packet = &packet;
  qDebug() << "FeedNotice:" << m_packet->command() << m_packet->raw();

  m_hooks->readFeed(packet);
}
