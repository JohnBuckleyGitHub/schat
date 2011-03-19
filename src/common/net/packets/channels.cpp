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

#include "Channel.h"
#include "net/PacketReader.h"
#include "net/packets/channels.h"
#include "net/PacketWriter.h"

JoinReply::JoinReply(Channel *channel)
  : Packet(Protocol::JoinReplyPacket)
  , m_channel(channel)
{
}


JoinReply::JoinReply(PacketReader *reader)
  : Packet(reader)
{
  m_channel = new Channel();
  m_channel->setId(reader->id());
  m_channel->setName(reader->text());
  m_channel->setDesc(reader->text());
  m_channel->setTopic(reader->text());
  m_channel->setUsers(reader->idList());
}


bool JoinReply::isValid() const
{
  return m_channel->isValid();
}


void JoinReply::body()
{
  m_writer->putId(m_channel->id());
  m_writer->put(m_channel->name());
  m_writer->put(m_channel->desc());
  m_writer->put(m_channel->topic());
  m_writer->putId(m_channel->users());
}
