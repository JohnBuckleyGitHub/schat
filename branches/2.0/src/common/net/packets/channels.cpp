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

ChannelPacket::ChannelPacket(ClientChannel channel, const QByteArray &dest, const QString &command, quint64 time)
  : Notice(channel->id(), dest, command, time)
  , m_channelId(channel->id())
  , m_name(channel->name())
  , m_users(channel->users())
{
  m_type = ChannelType;
}


ChannelPacket::ChannelPacket(quint16 type, PacketReader *reader)
  : Notice(type, reader)
{
  if (SimpleID::typeOf(reader->sender()) == SimpleID::ChannelId)
    m_channelId = reader->sender();
  else if (SimpleID::typeOf(reader->dest()) == SimpleID::ChannelId)
    m_channelId = reader->dest();

  m_name  = reader->text();
  m_users = reader->idList();
}


void ChannelPacket::write(PacketWriter *writer) const
{
  writer->put(m_name);
  writer->put(m_users);
}


ChannelWriter::ChannelWriter(QDataStream *stream, Channel *channel, const QByteArray &dest)
  : PacketWriter(stream, Protocol::ChannelPacket, channel->id(), dest)
{
  putId(channel->id());
  put(channel->name());
  put(channel->data());
  put(channel->topic().topic);
  putId(channel->users());
}


ChannelReader::ChannelReader(PacketReader *reader)
{
  channel = new Channel();
  channel->setId(reader->id());
  channel->setName(reader->text());
  channel->setData(reader->json().toMap());
  channel->setTopic(reader->text());
  channel->setUsers(reader->idList());
}
