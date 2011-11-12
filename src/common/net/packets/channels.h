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
#include "net/PacketWriter.h"
#include "User.h"

class SCHAT_EXPORT ChannelPacket : public Notice
{
public:
  ChannelPacket(ClientChannel channel, const QByteArray &dest, const QString &command, quint64 time = 0);
  ChannelPacket(quint16 type, PacketReader *reader);

  inline const QByteArray& channelId() const { return m_channelId; }
  inline const QList<QByteArray>& users() const { return m_users; }
  inline const QString& name() const { return m_text; }
  static QVariantMap feeds(ClientChannel channel, ClientUser user);

protected:
  void write(PacketWriter *writer) const;

  QByteArray m_channelId;          ///< Идентификатор канала.
  QList<QByteArray> m_users;       ///< Список идентификаторов пользователей в канале.
};

#endif /* CHANNELS_H_ */
