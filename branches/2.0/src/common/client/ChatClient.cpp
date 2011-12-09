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
#include "client/ClientChannels.h"
#include "client/ClientHooks.h"
#include "client/ClientMessages.h"
#include "client/SimpleClient.h"

ChatClient *ChatClient::m_self = 0;

ChatClient::ChatClient(QObject *parent)
  : QObject(parent)
{
  m_self = this;

  m_client = new SimpleClient(this);

  m_channels = new ClientChannels(this);
  m_messages = new ClientMessages(this);

  m_hooks = new Hooks::Client(this);
}


bool ChatClient::openId(const QByteArray &id)
{
  bool matched = false;
  return m_hooks->openId(id, &matched);
}


bool ChatClient::openUrl(const QUrl &url)
{
  return m_client->openUrl(url);
}


ClientChannel ChatClient::getChannel()
{
  return m_client->channel();
}


int ChatClient::getState()
{
  return m_client->clientState();
}


/*!
 * Получение идентификатора пользователя.
 *
 * \sa Hooks::Client::id()
 */
QByteArray ChatClient::getId()
{
  QByteArray id = m_client->channelId();
  if (!id.isEmpty())
    return id;

  id = m_client->channel()->id();
  if (!id.isEmpty())
    return id;

  return m_hooks->id();
}


/*!
 * Получение идентификатора сервера.
 * Если у клиента не установлен идентификатор, используются хуки для его получения.
 *
 * \sa Hooks::Client::serverId()
 */
QByteArray ChatClient::getServerId()
{
  QByteArray id = m_client->serverId();
  if (!id.isEmpty())
    return id;

  return m_hooks->serverId();
}