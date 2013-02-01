/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include "DateTime.h"
#include "net/Channels.h"
#include "net/PacketReader.h"
#include "net/packets/ChannelNotice.h"
#include "net/PacketWriter.h"
#include "sglobal.h"

ChannelNotice::ChannelNotice(const QByteArray &sender, const QByteArray &dest, const QString &command, quint64 time)
  : Notice(sender, dest, command, time)
  , m_gender(0)
  , m_channelStatus(0)
{
  m_type = ChannelType;
}


ChannelNotice::ChannelNotice(quint16 type, PacketReader *reader)
  : Notice(type, reader)
  , m_gender(0)
  , m_channelStatus(0)
{
  if (m_direction == Server2Client)
    m_channelId = reader->sender();
  else
    m_channelId = reader->dest();

  m_gender   = reader->get<quint8>();
  m_channelStatus   = reader->get<quint8>();
  m_channels = reader->idList();
}


void ChannelNotice::write(PacketWriter *writer) const
{
  writer->put(m_gender);
  writer->put(m_channelStatus);
  writer->putId(m_channels);
}


/*!
 * Формирование пакета для отправки клиенту заголовка канала.
 * Также будут отосланы заголовки фидов в соответствии с правами \p user.
 *
 * \param channel Канал.
 * \param user    Получатель.
 * \param command Команда.
 */
ChannelPacket ChannelNotice::channel(ClientChannel channel, ClientChannel user, const QString &command)
{
  ChannelPacket packet(new ChannelNotice(channel->id(), user->id(), command, DateTime::utc()));
  packet->setDirection(Server2Client);
  packet->setText(channel->name());
  packet->m_gender        = channel->gender().raw();
  packet->m_channelStatus = channel->status().value();
  packet->setData(channel->feeds().f(user.data()));

  if (channel->type() == SimpleID::ChannelId)
    packet->m_channels = channel->channels().all();

  return packet;
}


/*!
 * Формирование пакета для отправки клиенту заголовка канала.
 *
 * \param channel Канал.
 * \param dest    Идентификатор получателя.
 * \param command Команда.
 */
ChannelPacket ChannelNotice::channel(ClientChannel channel, const QByteArray &dest, const QString &command)
{
  ChannelPacket packet(new ChannelNotice(channel->id(), dest, command, DateTime::utc()));
  packet->setDirection(Server2Client);
  packet->setText(channel->name());
  packet->m_gender        = channel->gender().raw();
  packet->m_channelStatus = channel->status().value();
//  packet.setData(channel->feeds().headers(0));

  if (channel->type() == SimpleID::ChannelId)
    packet->m_channels = channel->channels().all();

  return packet;
}


ChannelPacket ChannelNotice::info(ClientChannel channel)
{
  ChannelPacket packet(new ChannelNotice(channel->id(), channel->id(), CHANNELS_INFO_CMD, DateTime::utc()));
  packet->setDirection(Server2Client);
  packet->setText(channel->name());
  packet->m_gender        = channel->gender().raw();
  packet->m_channelStatus = channel->status().value();
  return packet;
}


/*!
 * Формирование пакета "info" для запроса информации о каналах.
 *
 * \param user     Идентификатор пользователя отправителя.
 * \param channels Список идентификаторов каналов, о которых необходима информация.
 */
ChannelPacket ChannelNotice::info(const QByteArray &user, const QList<QByteArray> &channels)
{
  ChannelPacket packet(new ChannelNotice(user, user, CHANNELS_INFO_CMD, DateTime::utc()));
  packet->m_channels = channels;
  return packet;
}


/*!
 * Базовый пакет ответа за запрос клиента.
 */
ChannelPacket ChannelNotice::reply(const ChannelNotice &source, int status)
{
  ChannelPacket packet(new ChannelNotice(source.dest(), source.sender(), source.command()));
  packet->setDirection(Server2Client);
  packet->setText(source.text());
  packet->setStatus(status);
  return packet;
}


/*!
 * Базовая функция формирования запроса к серверу.
 */
ChannelPacket ChannelNotice::request(const QByteArray &user, const QByteArray &channel, const QString &command, const QString &text)
{
  ChannelPacket packet(new ChannelNotice(user, channel, command));
  packet->setText(text);
  return packet;
}


/*!
 * Отправка обновлённой информации о себе.
 */
ChannelPacket ChannelNotice::update(ClientChannel channel)
{
  ChannelPacket packet(new ChannelNotice(channel->id(), channel->id(), LS("update"), DateTime::utc()));
  packet->setText(channel->name());
  packet->m_gender        = channel->gender().raw();
  packet->m_channelStatus = channel->status().value();
  return packet;
}
