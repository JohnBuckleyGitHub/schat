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

#include "debugstream.h"
#include "net/PacketReader.h"
#include "net/packets/message.h"
#include "net/PacketWriter.h"
#include "net/Protocol.h"


Message::Message(const QString &command, const QString &message)
  : Packet(Protocol::MessagePacket)
  , m_command(command)
  , m_message(message)
  , m_options(ControlMessage)
{
}


Message::Message(const QString &message)
  : Packet(Protocol::MessagePacket)
  , m_message(message)
  , m_options(GenericMessage)
{
}


Message::Message(PacketReader *reader)
  : Packet(reader)
{
  m_options = reader->get<quint8>();
  if (m_options & ControlMessage)
    m_command = reader->text();

  m_message = reader->text();
}


void Message::body()
{
  if (!m_command.isEmpty())
    m_options |= ControlMessage;

  m_writer->put(m_options);

  if (m_options & ControlMessage)
    m_writer->put(m_command);

  m_writer->put(m_message);
}
