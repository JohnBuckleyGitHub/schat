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

#include <QDebug>

#include "cores/Core.h"
#include "feeds/FeedStorage.h"
#include "net/PacketReader.h"
#include "net/packets/FeedPacket.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "NodeFeeds.h"
#include "Storage.h"

NodeFeeds::NodeFeeds(Core *core)
  : NodeNoticeReader(Notice::FeedType, core)
{
}


bool NodeFeeds::read(PacketReader *reader)
{
  if (SimpleID::typeOf(reader->sender()) != SimpleID::UserId)
    return false;

  m_user = m_storage->channel(reader->sender(), SimpleID::UserId);
  if (!m_user)
    return false;

  m_channel = m_storage->channel(reader->dest(), SimpleID::typeOf(reader->dest()));
  if (!m_channel)
    return false;

  FeedPacket packet(m_type, reader);
  m_packet = &packet;

  QString cmd = m_packet->command();
  qDebug() << "NodeFeeds::read()" << cmd;

  int status = Notice::NotImplemented;

  if (cmd == "headers")
    return headers();
  else if (cmd == "get")
    return get();
  else if (cmd == "update")
    return update();
  else if (cmd == "clear")
    return clear();
  else if (cmd == "query")
    return query();
  else if (cmd == "add")
    return add();
  else if (cmd == "remove")
    return remove();
  else if (cmd == "revert") {
    status = revert();
  }

  if (status == Notice::OK)
    return false;

  reply(status);
  return false;
}


bool NodeFeeds::add()
{
  int status = m_channel->feeds().add(m_packet->text(), m_packet->json(), m_user.data());
  m_core->send(m_user->sockets(), FeedPacket::reply(*m_packet, status, m_core->sendStream()));
  return false;
}


bool NodeFeeds::clear()
{
  int status = m_channel->feeds().clear(m_packet->text(), m_user.data());
  m_core->send(m_user->sockets(), FeedPacket::reply(*m_packet, status, m_core->sendStream()));
  return false;
}


bool NodeFeeds::get()
{
  if (m_packet->text().isEmpty())
    return false;

  m_core->send(m_user->sockets(), FeedPacket::feed(m_channel, m_user, m_packet->text(), m_core->sendStream()));
  return false;
}


bool NodeFeeds::headers()
{
  m_core->send(m_user->sockets(), FeedPacket::headers(m_channel, m_user, m_core->sendStream()));
  return false;
}


bool NodeFeeds::query()
{
  FeedQueryReply reply = m_channel->feeds().query(m_packet->text(), m_packet->json(), m_user.data());
  m_core->send(m_user->sockets(), FeedPacket::reply(*m_packet, reply, m_core->sendStream()));
  return false;
}


bool NodeFeeds::remove()
{
  int status = m_channel->feeds().remove(m_packet->text(), m_user.data());
  m_core->send(m_user->sockets(), FeedPacket::reply(*m_packet, status, m_core->sendStream()));
  return false;
}


bool NodeFeeds::update()
{
  int status = m_channel->feeds().update(m_packet->text(), m_packet->json(), m_user.data());
  m_core->send(m_user->sockets(), FeedPacket::reply(*m_packet, status, m_core->sendStream()));
  return false;
}


int NodeFeeds::revert()
{
  QString name = m_packet->text();
  if (name.isEmpty())
    return Notice::BadRequest;

  if (!m_channel->feeds().all().contains(name))
    return Notice::NotFound;

  FeedPtr feed = m_channel->feeds().all().value(name);
  int status = FeedStorage::revert(feed, m_packet->json());
  if (status == Notice::OK)
    reply(status);

  return status;
}


void NodeFeeds::reply(int status)
{
  m_core->send(m_user->sockets(), FeedPacket::reply(*m_packet, status, m_core->sendStream()));
}
