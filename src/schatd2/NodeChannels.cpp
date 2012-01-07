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

#include <QDebug>
#include "debugstream.h"

#include "cores/Core.h"
#include "events.h"
#include "net/PacketReader.h"
#include "net/packets/ChannelPacket.h"
#include "net/packets/Notice.h"
#include "NodeChannels.h"
#include "Normalize.h"
#include "Sockets.h"
#include "Storage.h"

NodeChannels::NodeChannels(Core *core)
  : NodeNoticeReader(Notice::ChannelType, core)
{
}


bool NodeChannels::read(PacketReader *reader)
{
  if (SimpleID::typeOf(reader->sender()) != SimpleID::UserId)
    return false;

  ChannelPacket packet(m_type, reader);
  m_packet = &packet;

  QString cmd = m_packet->command();
  qDebug() << "NodeChannels::read" << cmd;

  if (cmd == "info")
    return info();

  if (cmd == "join")
    return join();

  if (cmd == "-")
    return part();

  if (cmd == "quit")
    return quit();

  if (cmd == "update")
    return update();

  return false;
}


void NodeChannels::acceptImpl(ChatChannel user, const AuthResult &result, QList<QByteArray> &packets)
{
  Q_UNUSED(result)

  packets.append(ChannelPacket::channel(user, user, m_core->sendStream()));
}


void NodeChannels::addImpl(ChatChannel user)
{
  m_core->send(Sockets::all(user), ChannelPacket::channel(user, user->id(), m_core->sendStream(), "info"));
}


void NodeChannels::releaseImpl(ChatChannel user, quint64 socket)
{
  Q_UNUSED(socket);

  if (user->sockets().size())
    return;

  m_core->send(Sockets::all(user), ChannelPacket::quit(user->id(), m_core->sendStream()));

  QList<QByteArray> channels = user->channels().all();
  foreach (QByteArray id, channels) {
    ChatChannel channel = m_storage->channel(id);
    if (channel && channel->type() == SimpleID::ChannelId) {
      channel->channels().remove(user->id());
      user->channels().remove(channel->id());
      m_storage->gc(channel);
    }
  }
}


/*!
 * Обработка запроса пользователем информации о канале.
 */
bool NodeChannels::info()
{
  ChatChannel user = m_storage->channel(m_packet->sender(), SimpleID::UserId);
  if (!user)
    return false;

  if (m_packet->channels().isEmpty())
    return false;

  QList<QByteArray> packets;
  foreach (QByteArray id, m_packet->channels()) {
    ChatChannel channel = m_storage->channel(id, SimpleID::typeOf(id));
    if (channel)
      packets += ChannelPacket::channel(channel, user, m_core->sendStream(), "info");
  }

  if (packets.isEmpty())
    return false;

  m_core->send(user->sockets(), packets);
  return false;
}


/*!
 * Обработка запроса пользователя подключения к каналу.
 */
bool NodeChannels::join()
{
  ChatChannel user = m_storage->channel(m_packet->sender(), SimpleID::UserId);
  if (!user)
    return false;

  ChatChannel channel;

  /// Если идентификатор канала корректный функция пытается получить его по этому идентификатору.
  int type = SimpleID::typeOf(m_packet->channelId());
  if (type != SimpleID::InvalidId)
    channel = m_storage->channel(m_packet->channelId(), type);

  /// Если канал не удалось получить по идентификатору, будет произведена попытка создать обычный канал по имени.
  if (!channel)
    channel = m_storage->channel(m_packet->text());

  if (!channel)
    return false;

  bool notify = !channel->channels().all().contains(user->id());
  channel->channels() += user->id();
  user->channels()    += channel->id();

  m_core->send(user->sockets(), ChannelPacket::channel(channel, user, m_core->sendStream()));

  /// В случае необходимости всем пользователям в канале будет разослано уведомление в входе нового пользователя.
  if (notify && channel->channels().all().size() > 1 && channel->type() == SimpleID::ChannelId)
    m_core->send(Sockets::channel(channel), ChannelPacket::channel(user, channel->id(), m_core->sendStream(), "+"));

  return false;
}


/*!
 * Обработка отключения пользователя от канала.
 */
bool NodeChannels::part()
{
  ChatChannel user = m_storage->channel(m_packet->sender(), SimpleID::UserId);
  if (!user)
    return false;

  ChatChannel channel = m_storage->channel(m_packet->channelId(), SimpleID::typeOf(m_packet->channelId()));
  if (!channel)
    return false;

  user->channels().remove(channel->id());

  if (!channel->channels().all().contains(user->id()))
    return false;

  m_core->send(Sockets::channel(channel), ChannelPacket::part(user->id(), channel->id(), m_core->sendStream()));
  channel->channels().remove(user->id());

  m_storage->gc(channel);

  return false;
}


bool NodeChannels::quit()
{
  m_core->send(QList<quint64>() << m_core->packetsEvent()->socket(), QByteArray(), NewPacketsEvent::KillSocketOption);
  return false;
}


/*!
 * Обработка обновления информации о пользователе.
 */
bool NodeChannels::update()
{
  ChatChannel user = m_storage->channel(m_packet->sender(), SimpleID::UserId);
  if (!user)
    return false;

  if (!Channel::isValidName(m_packet->text()))
    return false;

  if (m_packet->channelStatus() == Status::Offline) {
    m_core->send(QList<quint64>() << m_core->packetsEvent()->socket(), QByteArray(), NewPacketsEvent::KillSocketOption);
    return false;
  }

  int updates = 0;

  if (user->name() != m_packet->text()) {
    m_storage->rename(user, m_packet->text());
    updates++;
  }

  if (user->gender().raw() != m_packet->gender()) {
    user->gender() = m_packet->gender();
    updates++;
  }

  if (user->status().value() != m_packet->channelStatus()) {
    user->status() = m_packet->channelStatus();
    updates++;
  }

  if (!updates)
    return false;

  m_core->send(Sockets::all(user, true), ChannelPacket::channel(user, user->id(), m_core->sendStream(), "info"));
  return false;
}
