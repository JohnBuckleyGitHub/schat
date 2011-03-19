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

#include "net/Packet.h"
#include "net/Protocol.h"
#include "net/PacketReader.h"
#include "net/PacketWriter.h"


Packet::Packet(PacketReader *reader)
  : m_reader(reader)
  , m_writer(0)
  , m_dest(reader->dest())
  , m_sender(reader->sender())
  , m_type(reader->type())
  , m_subtype(reader->subtype())
{
}


Packet::Packet(quint16 type)
  : m_reader(0)
  , m_writer(0)
  , m_type(type)
  , m_subtype(0)
{
}


Packet::Packet(quint16 type, const QByteArray &sender, const QByteArray &dest)
  : m_reader(0)
  , m_writer(0)
  , m_dest(dest)
  , m_sender(sender)
  , m_type(type)
  , m_subtype(0)
{
}


QByteArray Packet::data(QDataStream *stream)
{
  PacketWriter writer(stream, m_type, m_sender, m_dest);
  m_writer = &writer;
  body();
  return writer.data();
}


int Packet::idType(const QByteArray &id)
{
  if (id.size() != Protocol::IdSize)
    return Protocol::InvalidId;

  return id.at(Protocol::IdSize - 1);
}
