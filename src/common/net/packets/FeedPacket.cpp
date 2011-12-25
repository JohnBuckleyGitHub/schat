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

#include "DateTime.h"
#include "net/packets/FeedPacket.h"

FeedPacket::FeedPacket()
  : Notice(QByteArray(), QByteArray(), QString())
{
  m_type = FeedType;
}


FeedPacket::FeedPacket(const QByteArray &sender, const QByteArray &dest, const QString &command, const QByteArray &id)
  : Notice(sender, dest, command, DateTime::utc(), id)
{
  m_type = FeedType;
}


FeedPacket::FeedPacket(quint16 type, PacketReader *reader)
  : Notice(type, reader)
{
}


QByteArray FeedPacket::headers(const QByteArray &user, const QByteArray &channel, QDataStream *stream)
{
  FeedPacket packet(user, channel, "headers");
  return packet.data(stream);
}
