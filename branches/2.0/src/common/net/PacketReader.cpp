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

#include "net/PacketReader.h"
#include "net/Protocol.h"

PacketReader::PacketReader(QDataStream *stream)
  : m_stream(stream),
    m_device(stream->device())
{
  m_device->seek(0);
  *stream >> m_type >> m_subtype >> m_option;

  if (m_option & Protocol::SenderField)
    m_sender = id();

  if (m_option & Protocol::ChannelField)
    m_channel = id();

  if (m_option & Protocol::DestinationField)
    m_dest = idList();
}


bool PacketReader::isMulticast() const
{
  if (m_dest.isEmpty())
    return false;

  if (is(Protocol::Multicast))
    return true;

  if (SimpleID::typeOf(dest()))
    return true;

  return false;
}
