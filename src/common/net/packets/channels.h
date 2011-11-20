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

#ifndef CHANNELS_H_
#define CHANNELS_H_

#include "Channel.h"
#include "net/packets/notices.h"

class PacketReader;
class PacketWriter;

class SCHAT_EXPORT ChannelPacket : public Notice
{
public:
  ChannelPacket(const QByteArray &sender, const QByteArray &dest, const QString &command, quint64 time = 0);
  ChannelPacket(quint16 type, PacketReader *reader);

  inline ClientChannel channel() const { return m_channel; }
  inline quint8 gender() const { return m_gender; }
  inline quint8 status() const { return m_status; }

  static QByteArray channel(ClientChannel channel, const QByteArray &dest, QDataStream *stream);
  static QByteArray s2c(ClientChannel channel, const QByteArray &dest, const QByteArray &command, QDataStream *stream);

protected:
  void write(PacketWriter *writer) const;

  quint8 m_gender;         ///< Пол и цвет пользователя.
  quint8 m_status;         ///< Базовый статус пользователя.
  ClientChannel m_channel; ///< Канал.
};

#endif /* CHANNELS_H_ */
