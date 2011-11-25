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

#include <QDebug>

#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/SimpleClient.h"
#include "net/packets/channels.h"

ClientChannels::ClientChannels(QObject *parent)
  : QObject(parent)
  , m_client(ChatClient::io())
{
  connect(m_client, SIGNAL(notice(int)), SLOT(notice(int)));
}


/*!
 * Подключение к обычному каналу по имени.
 *
 * \param name Имя канала.
 */
void ClientChannels::join(const QString &name)
{
  if (!Channel::isValidName(name))
    return;

  m_client->send(ChannelPacket::join(ChatClient::id(), QByteArray(), name, m_client->sendStream()));
}


void ClientChannels::notice(int type)
{
  qDebug() << "NOTICE" << type;

  ChannelPacket notice(type, ChatClient::io()->reader());
  if (!notice.isValid())
    return;

  qDebug() << notice.command();
}
