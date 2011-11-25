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

#include "cores/Core.h"
#include "net/PacketReader.h"
#include "net/packets/channels.h"
#include "net/packets/notices.h"
#include "NodeChannels.h"
#include "Storage.h"

NodeChannels::NodeChannels(Core *core)
  : NodeNoticeReader(Notice::ChannelType, core)
{
}


bool NodeChannels::read(PacketReader *reader)
{
  if (SimpleID::typeOf(reader->sender()) != SimpleID::UserId)
    return false;

  ChannelPacket packet(m_type, reader);
  m_packet = &packet;

  QString cmd = m_packet->command();

  if (cmd == "join")
    return join();

  return false;
}


/*!
 * Обработка запроса пользователя подключения к каналу.
 */
bool NodeChannels::join()
{
  ChatChannel user = m_storage->channel(m_packet->sender(), SimpleID::UserId);
  if (!user)
    return false;

  ChatChannel channel;
  if (Channel::isCompatibleId(m_packet->dest()) != SimpleID::InvalidId)
    channel = m_storage->channel(m_packet->dest(), SimpleID::typeOf(m_packet->dest()));

  if (!channel)
    channel = m_storage->channel(m_packet->text());

  if (!channel)
    return false;

  channel->channels().add(user->id());
  user->channels().add(channel->id());

  m_core->send(user->sockets(), ChannelPacket::channel(channel, channel->id(), m_core->sendStream()));

  return false;
}
