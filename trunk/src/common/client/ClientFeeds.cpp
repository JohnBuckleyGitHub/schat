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

#include "client/ChatClient.h"
#include "client/ClientFeeds.h"
#include "client/ClientHooks.h"
#include "client/SimpleClient.h"
#include "net/packets/FeedPacket.h"
#include "net/packets/Notice.h"

ClientFeeds::ClientFeeds(QObject *parent)
  : QObject(parent)
{
  m_hooks = new Hooks::Feeds(this);

  connect(ChatClient::io(), SIGNAL(notice(int)), SLOT(notice(int)));
}


bool ClientFeeds::get(const QByteArray &id, const QString &name)
{
  if (!Channel::isCompatibleId(id))
    return false;

  if (name.isEmpty())
    return false;

  return ChatClient::io()->send(FeedPacket::get(ChatClient::id(), id, name, ChatClient::io()->sendStream()));
}


/*!
 * Запрос заголовков фидов.
 */
bool ClientFeeds::headers(const QByteArray &id)
{
  if (!Channel::isCompatibleId(id))
    return false;

  return ChatClient::io()->send(FeedPacket::headers(ChatClient::id(), id, ChatClient::io()->sendStream()));
}


bool ClientFeeds::update(const QByteArray &id, const QString &name, const QVariantMap &json)
{
  if (!Channel::isCompatibleId(id))
    return false;

  if (name.isEmpty())
    return false;

  if (json.isEmpty())
    return false;

  return ChatClient::io()->send(FeedPacket::update(ChatClient::id(), id, name, json, ChatClient::io()->sendStream()));
}


void ClientFeeds::notice(int type)
{
  if (type != Notice::FeedType)
    return;

  FeedPacket packet(type, ChatClient::io()->reader());
  if (!packet.isValid())
    return;

  m_packet = &packet;
  qDebug() << "FeedPacket:" << m_packet->command() << m_packet->raw();

  m_hooks->readFeed(packet);
}
