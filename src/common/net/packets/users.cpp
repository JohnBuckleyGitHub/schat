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
#include "net/PacketWriter.h"
#include "User.h"

UserData::UserData(PacketReader *reader)
  : Packet(reader)
{
  m_options = reader->get<quint8>();
  reader->get<quint8>(); // reserved.

  m_user = new User();
  m_user->setId(sender());
  m_user->setNick(reader->text());
}


UserData::UserData(User *user, const QByteArray &channelId)
  : Packet(Protocol::UserDataPacket, user->id(), channelId)
  , m_options(0)
  , m_user(user)
{
}


bool UserData::isValid() const
{
  return m_user->isValid();
}


void UserData::body()
{
  m_writer->put(m_options);
  m_writer->put(quint8(0));
  m_writer->put(m_user->nick());
}
