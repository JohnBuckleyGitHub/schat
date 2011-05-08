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

#include "net/packets/users.h"
#include "net/PacketReader.h"
#include "User.h"

UserWriter::UserWriter(QDataStream *stream, User *user, const QByteArray &destId, int options)
  : PacketWriter(stream, Protocol::UserDataPacket, user->id(), destId)
{
  put<quint8>(options);
  put<quint8>(0);
  put(user->nick());
}


UserReader::UserReader(PacketReader *reader)
{
  options = reader->get<quint8>();
  reader->get<quint8>(); // reserved.

  user = new User();
  user->setId(reader->sender());
  user->setNick(reader->text());
}
