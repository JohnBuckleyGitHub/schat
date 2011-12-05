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

#include <qplugin.h>

#include "BotEchoPlugin.h"
#include "BotEchoPlugin_p.h"
#include "client/ClientHelper.h"
#include "client/SimpleClient.h"
#include "FileLocations.h"
#include "net/packets/messages.h"

BotEcho::BotEcho(ClientHelper *helper, FileLocations *locations)
  : QObject(helper)
  , m_helper(helper)
  , m_locations(locations)
  , m_client(helper->client())
{
  connect(m_client, SIGNAL(join(const QByteArray &, const QByteArray &)), SLOT(join(const QByteArray &, const QByteArray &)));
  connect(m_client, SIGNAL(synced(const QByteArray &)), SLOT(synced(const QByteArray &)));
  connect(m_client, SIGNAL(message(const MessageData &)), SLOT(message(const MessageData &)));
}


void BotEcho::join(const QByteArray &channelId, const QByteArray &userId)
{
  if (userId.isEmpty() || userId == m_client->channelId())
    return;

  ClientUser user = m_client->user(userId);
  if (!user)
    return;

  MessageData message(m_client->channelId(), channelId, QString(), QString("Hello <b>%1</b>!").arg(user->nick()));
  m_client->send(message);
}


void BotEcho::message(const MessageData &data)
{
  ClientUser user = m_client->user(data.senderId);
  if (!user)
    return;

  if (data.destId() != m_client->channelId())
    return;

  MessageData message(m_client->channelId(), data.senderId, data.command, data.text);
  m_client->send(message);
}


void BotEcho::synced(const QByteArray &channelId)
{
  MessageData message(m_client->channelId(), channelId, QString(), QString("Hello! I am <b>%1</b>").arg(m_client->nick()));
  m_client->send(message);
}


QObject *BotEchoPlugin::init(ClientHelper *helper, FileLocations *locations)
{
  m_echo = new BotEcho(helper, locations);
  return m_echo;
}


Q_EXPORT_PLUGIN2(BotEcho, BotEchoPlugin);
