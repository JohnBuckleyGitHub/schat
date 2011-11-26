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
  if (m_direction == Server2Client)
    m_channelId = reader->sender();
  else
    m_channelId = reader->dest();

  m_gender = reader->get<quint8>();
  m_status = reader->get<quint8>();

  if (SimpleID::typeOf(m_channelId) == SimpleID::ChannelId && m_command == "channel")
    m_channels = reader->idList();
}


void ChannelPacket::write(PacketWriter *writer) const
{
  writer->put(m_gender);
  writer->put(m_status);

  if (!m_channels.isEmpty() && m_command == "channel")
    writer->putId(m_channels);
}


/*!
 * Формирование пакета \b channel для отправки клиенту заголовка канала.
 *
 * \param channel Канал.
 * \param dest    Идентификатор получателя.
 * \param stream  Поток записи пакета.
 */
QByteArray ChannelPacket::channel(ClientChannel channel, const QByteArray &dest, QDataStream *stream)
{
  ChannelPacket packet(channel->id(), dest, "channel", DateTime::utc());
  packet.setDirection(Server2Client);
  packet.setText(channel->name());
  packet.m_gender   = channel->gender().raw();
  packet.m_status   = channel->status().value();
  packet.setData(channel->feeds().get(channel.data()));

  if (channel->type() == SimpleID::ChannelId)
    packet.m_channels = channel->channels().all();

  return packet.data(stream);
}


QByteArray ChannelPacket::join(const QByteArray &user, const QByteArray &channel, const QString &name, QDataStream *stream)
{
  ChannelPacket packet(user, channel, "join", DateTime::utc());
  packet.setText(name);
  return packet.data(stream);
}
