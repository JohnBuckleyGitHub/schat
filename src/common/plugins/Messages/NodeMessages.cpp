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

#include "Ch.h"
#include "cores/Core.h"
#include "DateTime.h"
#include "feeds/FeedEvents.h"
#include "feeds/FeedStorage.h"
#include "net/PacketReader.h"
#include "net/packets/MessageNotice.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "NodeMessages.h"
#include "NodeMessagesDB.h"
#include "sglobal.h"

NodeMessages::NodeMessages(Core *core)
  : NodeNoticeReader(Notice::MessageType, core)
  , m_packet(0)
{
}


/*!
 * Чтение нового входящего сообщения.
 */
bool NodeMessages::read(PacketReader *reader)
{
  if (SimpleID::typeOf(reader->sender()) != SimpleID::UserId)
    return false;

  m_sender = Ch::channel(reader->sender(), SimpleID::UserId);
  if (!m_sender)
    return false;

  MessageNotice packet(m_type, reader);
  m_packet = &packet;

  FeedEvent *event = createEvent();

  if (!m_dest)
    event->status = Notice::NotFound;
  else if (m_dest->type() == SimpleID::ServerId)
    event->status = Notice::BadRequest;
  else if (!m_dest->canWrite(m_sender))
    event->status = Notice::Forbidden;

  if (event->status != Notice::OK) {
    reject(event->status);
    FeedEvents::start(event);
    return false;
  }

  if (packet.direction() == Notice::Internal) {
    Core::i()->route(m_dest);
    delete event;
    return false;
  }

  FeedPtr feed  = m_dest->feed(FEED_NAME_MESSAGES, true, false);
  event->diffTo = event->date;
  event->date   = m_packet->date();

  if (m_dest->type() == SimpleID::UserId && m_dest->status().value() == Status::Offline) {
    event->status = Notice::ChannelOffline;
    reject(event->status);

    NodeMessagesDB::add(packet, event->status);
    Ch::gc(m_dest);
  }
  else {
    NodeMessagesDB::add(packet);
    Core::i()->route(m_dest);
  }

  FeedStorage::save(feed, m_packet->date());
  FeedEvents::start(event);
  return false;
}


/*!
 * Создание события для уведомления об операции с фидом \b messages
 * и установка внутреннего и публичного идентификатора сообщения и его даты.
 */
FeedEvent *NodeMessages::createEvent()
{
  m_packet->setDate(Core::date());
  m_packet->setInternalId(m_packet->id());
  m_packet->setId(m_packet->toId());

  FeedEvent *event = new FeedEvent(m_packet->dest(), m_packet->sender(), FEED_METHOD_POST);
  event->name      = FEED_NAME_MESSAGES;
  event->request   = m_packet->json();
  event->socket    = Core::socket();
  event->status    = Notice::OK;
  event->path      = SimpleID::encode(m_packet->id());

  m_dest = Ch::channel(m_packet->dest(), SimpleID::typeOf(m_packet->dest()));
  if (m_dest)
    event->date = m_dest->feed(FEED_NAME_MESSAGES, true, false)->head().date();

  return event;
}


void NodeMessages::reject(int status)
{
  MessageNotice packet(m_packet->sender(), m_packet->dest(), m_packet->text(), m_packet->date(), m_packet->internalId());
  packet.setStatus(status);
  m_core->send(m_sender->sockets(), packet.data(Core::stream()));
}
