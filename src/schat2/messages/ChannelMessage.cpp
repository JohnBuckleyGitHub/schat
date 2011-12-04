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
#include "messages/ChannelMessage.h"
#include "net/packets/MessagePacket.h"
#include "net/SimpleID.h"

ChannelMessage::ChannelMessage(const MessagePacket &packet)
  : Message()
  , m_packet(packet)
{
  m_data["type"] = "channel";
  m_data["id"]   = QString(SimpleID::encode(packet.id()));
  m_data["text"] = packet.text();

  m_tab = detectTab();
}


QByteArray ChannelMessage::detectTab() const
{
  if (SimpleID::typeOf(m_packet.dest()) == SimpleID::ChannelId)
    return m_packet.dest();

  if (m_packet.sender() == ChatClient::id())
    return m_packet.dest();

  return m_packet.sender();
}
