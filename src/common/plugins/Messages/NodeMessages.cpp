/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include "Ch.h"
#include "cores/Core.h"
#include "DateTime.h"
#include "net/PacketReader.h"
#include "net/packets/MessageNotice.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "NodeMessages.h"
#include "NodeMessagesDB.h"

NodeMessages::NodeMessages(Core *core)
  : NodeNoticeReader(Notice::MessageType, core)
  , m_packet(0)
{
}


bool NodeMessages::read(PacketReader *reader)
{
  if (SimpleID::typeOf(reader->sender()) != SimpleID::UserId)
    return false;

  m_sender = Ch::channel(reader->sender(), SimpleID::UserId);
  if (!m_sender)
    return false;

  MessageNotice packet(m_type, reader);
  m_packet = &packet;

  m_dest = Ch::channel(reader->dest(), SimpleID::typeOf(reader->dest()));
  if (!m_dest) {
    reject(Notice::NotFound);
    return false;
  }
  else if (m_dest->type() == SimpleID::ServerId) {
    reject(Notice::BadRequest);
    return false;
  }
  else if (!m_dest->canWrite(m_sender)) {
    reject(Notice::Forbidden);
    return false;
  }

  if (packet.direction() == Notice::Internal) {
    Core::i()->route(m_dest);
    return false;
  }

  m_packet->setDate(Core::date());

  if (m_dest->type() == SimpleID::UserId && m_dest->status().value() == Status::Offline) {
    reject(Notice::ChannelOffline);

    m_packet->setId(m_packet->toId());
    NodeMessagesDB::add(packet, Notice::ChannelOffline);
    Ch::gc(m_dest);
    return false;
  }

  m_packet->setId(m_packet->toId());
  NodeMessagesDB::add(packet);

  Core::i()->route(m_dest);
  return false;
}


void NodeMessages::reject(int status)
{
  MessageNotice packet(m_packet->sender(), m_packet->dest(), m_packet->text(), Core::date(), m_packet->id());
  packet.setStatus(status);
  m_core->send(m_sender->sockets(), packet.data(Core::stream()));
}
