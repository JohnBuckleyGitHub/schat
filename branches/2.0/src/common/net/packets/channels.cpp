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

#include "DateTime.h"
#include "net/PacketReader.h"
#include "net/packets/channels.h"
#include "net/PacketWriter.h"

ChannelPacket::ChannelPacket(const QByteArray &sender, const QByteArray &dest, const QString &command, quint64 time)
  : Notice(sender, dest, command, time)
  , m_gender(0)
  , m_status(0)
{
  m_type = ChannelType;
}


ChannelPacket::ChannelPacket(quint16 type, PacketReader *reader)
  : Notice(type, reader)
  , m_gender(0)
  , m_status(0)
{
  m_channel = ClientChannel(new Channel());
  if (m_direction == Server2Client)
    m_channel->setId(reader->sender());
  else
    m_channel->setId(reader->dest());

  m_channel->setName(m_text);
  m_gender = reader->get<quint8>();
  m_status = reader->get<quint8>();

  m_channel->gender().setRaw(m_gender);
  m_channel->status().set(m_status);
}


void ChannelPacket::write(PacketWriter *writer) const
{
  writer->put(m_gender);
  writer->put(m_status);
}


/*!
 * Формирование пакета \b channel для отправки клиенту заголовка канала.
 */
QByteArray ChannelPacket::channel(ClientChannel channel, const QByteArray &dest, QDataStream *stream)
{
  return s2c(channel, dest, "channel", stream);
}


QByteArray ChannelPacket::s2c(ClientChannel channel, const QByteArray &dest, const QByteArray &command, QDataStream *stream)
{
  ChannelPacket packet(channel->id(), dest, command, DateTime::utc());
  packet.setDirection(Server2Client);
  packet.setText(channel->name());
  packet.setData(channel->feeds().json(channel.data(), false));
  packet.m_gender = channel->gender().raw();
  packet.m_status = channel->status().value();

  return packet.data(stream);
}
