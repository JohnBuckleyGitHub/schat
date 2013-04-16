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

#include <QTimer>

#include "Ch.h"
#include "cores/Core.h"
#include "DataBase.h"
#include "DateTime.h"
#include "events.h"
#include "feeds/ChannelFeed.h"
#include "feeds/FeedEvents.h"
#include "feeds/FeedsCore.h"
#include "feeds/NodeChannelFeed.h"
#include "feeds/ServerFeed.h"
#include "net/Channels.h"
#include "net/PacketReader.h"
#include "NodeChannels.h"
#include "NodeLog.h"
#include "Normalize.h"
#include "sglobal.h"
#include "Sockets.h"
#include "Storage.h"

NodeChannels::NodeChannels(Core *core)
  : QObject(core)
  , NodeNoticeReader(Notice::ChannelType, core)
  , m_packet(0)
{
  connect(FeedEvents::i(), SIGNAL(notify(FeedEvent)), SLOT(notify(FeedEvent)));
}


bool NodeChannels::read(PacketReader *reader)
{
  if (SimpleID::typeOf(reader->sender()) != SimpleID::UserId)
    return false;

  m_user = Ch::channel(reader->sender(), SimpleID::UserId);
  if (!m_user)
    return false;

  ChannelNotice packet(m_type, reader);
  m_packet = &packet;

  const QString cmd = m_packet->command();
  int status  = Notice::NotImplemented;

  SCHAT_LOG_DEBUG_STR("[GenericNode/Channels] read channel request, socket:" + QByteArray::number(Core::socket()) +
      ", id:" + SimpleID::encode(m_user->id()) +
      ", channelId:" + SimpleID::encode(m_packet->channelId) +
      ", cmd:" + cmd.toUtf8() +
      ", text:" + m_packet->text().toUtf8() +
      ", user:" + m_user->name().toUtf8())

  if (cmd == CHANNELS_INFO_CMD)
    return info();

  else if (cmd == CHANNELS_JOIN_CMD)
    return join(m_packet->channelId, m_packet->text());

  else if (cmd == CHANNELS_PART_CMD)
    return part();

  else if (cmd == CHANNELS_QUIT_CMD)
    return quit();

  else if (cmd == CHANNELS_UPDATE_CMD)
    status = update();

  else if (cmd == CHANNELS_NAME_CMD)
    status = name();

  if (status == Notice::OK)
    return false;

  reply(status);
  return false;
}


void NodeChannels::acceptImpl(ChatChannel user, const AuthResult & /*result*/, QList<QByteArray> &packets)
{
  m_user = user;
  packets.append(reply(Ch::server())->data(Core::stream()));
  packets.append(reply(m_user)->data(Core::stream()));

  m_user->channels().restore(DataBase::value(SimpleID::encode(m_user->id()) + LS("/channels")).toByteArray());

  if (m_user->channels().size())
    m_pending.append(user);

  if (m_pending.size() == 1)
    QTimer::singleShot(0, this, SLOT(join()));
}


void NodeChannels::addImpl(ChatChannel user)
{
  m_core->send(Sockets::all(user), ChannelNotice::info(user));
}


/*!
 * Обработка отключения пользователя.
 */
void NodeChannels::releaseImpl(ChatChannel user, quint64 socket)
{
  Q_UNUSED(socket);

  if (user->sockets().size())
    return;

  m_core->send(Sockets::all(user), ChannelNotice::request(user->id(), user->id(), CHANNELS_QUIT_CMD));

  QList<QByteArray> channels = user->channels().all();
  QByteArray data;

  foreach (const QByteArray &id, channels) {
    ChatChannel channel = Ch::channel(id);
    if (channel && channel->type() == SimpleID::ChannelId) {
      channel->removeChannel(user->id(), Ch::server()->feed(FEED_NAME_SERVER)->data().value(SERVER_FEED_OFFLINE_KEY, true).toBool());
      user->removeChannel(channel->id());
      data.append(channel->id());
      Ch::gc(channel);
    }
  }

  DataBase::setValue(SimpleID::encode(user->id()) + LS("/channels"), data);
}


void NodeChannels::join()
{
  while (!m_pending.isEmpty()) {
    m_user = m_pending.takeFirst();
    const QList<QByteArray> channels = m_user->channels().all(SimpleID::ChannelId);
    foreach (const QByteArray &id, channels) {
      join(id);
    }
  }
}


/*!
 * Обработка изменений фида FEED_NAME_CHANNEL.
 */
void NodeChannels::notify(const FeedEvent &event)
{
  if (event.status != Notice::OK || event.name != FEED_NAME_CHANNEL || !(event.method == FEED_METHOD_PUT || event.method == FEED_METHOD_POST) || !NodeChannelFeed::isReservedKey(event.path))
    return;

  ChatChannel channel = Ch::channel(event.channel, SimpleID::typeOf(event.channel));
  if (!channel)
    return;

  const QList<quint64> sockets = channel->type() == SimpleID::UserId ? Sockets::all(channel, true) : Sockets::channel(channel);
  m_core->send(sockets, ChannelNotice::info(channel, event.date));
}


/*!
 * Обработка запроса пользователем информации о канале.
 */
bool NodeChannels::info()
{
  if (m_packet->channels.isEmpty())
    return false;

  SCHAT_LOG_DEBUG_STR("[GenericNode/Channels] info, count:" + QByteArray::number(m_packet->channels.size()))

  QList<QByteArray> packets;
  foreach (const QByteArray &id, m_packet->channels) {
    ChatChannel channel = Ch::channel(id, SimpleID::typeOf(id));
    if (channel) {
      SCHAT_LOG_DEBUG_STR("[GenericNode/Channels] info, id:" + SimpleID::encode(channel->id()) + ", name:" + channel->name().toUtf8())

      if (channel->type() == SimpleID::UserId) {
        channel->addChannel(m_user->id());
        m_user->addChannel(channel->id());
      }

      packets += reply(channel, isForbidden(channel), CHANNELS_INFO_CMD)->data(Core::stream());
    }
  }

  if (packets.isEmpty())
    return false;

  m_core->send(m_user->sockets(), packets);
  return false;
}


/*!
 * Обработка запроса пользователя подключения к каналу.
 */
bool NodeChannels::join(const QByteArray &channelId, const QString &name)
{
  ChatChannel channel;

  /// Если идентификатор канала корректный, функция пытается получить его по этому идентификатору.
  const int type = SimpleID::typeOf(channelId);
  if (type != SimpleID::InvalidId)
    channel = Ch::channel(channelId, type);

  /// Если канал не удалось получить по идентификатору, будет произведена попытка создать обычный канал по имени.
  if (!channel && (type == SimpleID::InvalidId || type == SimpleID::ChannelId))
    channel = Ch::channel(name, m_user);

  if (!channel)
    return false;

  SCHAT_LOG_DEBUG_STR("[GenericNode/Channels] join, id:" + SimpleID::encode(channel->id()) + ", name:" + channel->name().toUtf8())

  if (channel->type() == SimpleID::ChannelId && isForbidden(channel)) {
    m_core->send(m_user->sockets(), reply(channel, true));
    return false;
  }

  const bool notify = !channel->channels().contains(m_user->id());
  channel->addChannel(m_user->id());
  m_user->addChannel(channel->id());

  m_core->send(m_user->sockets(), reply(channel));

  /// В случае необходимости всем пользователям в канале будет разослано уведомление в входе нового пользователя.
  if (notify && channel->channels().all().size() > 1 && channel->type() == SimpleID::ChannelId)
    m_core->send(Sockets::channel(channel), ChannelNotice::channel(m_user, channel->id(), CHANNELS_JOINED_CMD));

  return false;
}


/*!
 * Установка имени канала.
 *
 * \deprecated Этот метод изменения имени канала является устаревшим с версии 2.0.5.
 * Рекомендуется использовать \b put запрос к полю \b name фида \b channel.
 */
int NodeChannels::name()
{
  ChatChannel channel = Ch::channel(m_packet->channelId, SimpleID::typeOf(m_packet->channelId));
  if (!channel)
    return Notice::NotFound;

  FeedPtr feed = channel->feed(FEED_NAME_CHANNEL, false);
  if (!feed)
    return Notice::InternalError;

  return FeedsCore::put(channel.data(), CHANNEL_FEED_NAME_REQ, m_user.data(), m_packet->text(), Feed::Echo | Feed::Share | Feed::Broadcast).status;
}


/*!
 * Обработка отключения пользователя от канала.
 */
bool NodeChannels::part()
{
  ChatChannel channel = Ch::channel(m_packet->channelId, SimpleID::typeOf(m_packet->channelId));
  if (!channel)
    return false;

  m_user->removeChannel(channel->id());

  if (!channel->channels().all().contains(m_user->id()))
    return false;

  m_core->send(Sockets::channel(channel), ChannelNotice::request(m_user->id(), channel->id(), CHANNELS_PART_CMD));
  channel->removeChannel(m_user->id());

  Ch::gc(channel);
  return false;
}


bool NodeChannels::quit()
{
  m_core->send(QList<quint64>() << m_core->packetsEvent()->socket(), QByteArray(), NewPacketsEvent::KillSocketOption);
  return false;
}


/*!
 * Обработка получения обновлённой информации о пользователе.
 *
 * \deprecated Этот метод изменения информации о пользователе является устаревшим с версии 2.0.5.
 */
int NodeChannels::update()
{
  if (m_user->id() != m_packet->sender())
    return Notice::Forbidden;

  FeedPtr feed = m_user->feed(FEED_NAME_CHANNEL, false);
  if (!feed)
    return Notice::InternalError;

  int updates       = 0;
  const int options = Feed::Echo | Feed::Share | Feed::Broadcast;

  if (m_user->name() != m_packet->text() && FeedsCore::put(m_user.data(), CHANNEL_FEED_NAME_REQ, m_user.data(), m_packet->text(), options).status == Notice::OK)
    updates++;

  if (m_user->gender().raw() != m_packet->gender && FeedsCore::put(m_user.data(), CHANNEL_FEED_GENDER_REQ, m_user.data(), m_packet->gender, options).status == Notice::OK)
    updates++;

  if (m_user->status().value() != m_packet->channelStatus && FeedsCore::put(m_user.data(), CHANNEL_FEED_STATUS_REQ, m_user.data(), m_packet->channelStatus, options).status == Notice::OK)
    updates++;

  if (!updates)
    return Notice::NotModified;

  return Notice::OK;
}


/*!
 * Возвращает \b true если пользователю запрещён доступ в канала \p channel.
 */
bool NodeChannels::isForbidden(ChatChannel channel) const
{
  FeedPtr feed = channel->feed(FEED_NAME_ACL, false);
  return (feed && !feed->can(m_user.data(), Acl::Read));
}


/*!
 * Отправка информации о канале.
 */
ChannelPacket NodeChannels::reply(ChatChannel channel, bool forbidden, const QString &command) const
{
  ChannelPacket packet(new ChannelNotice(channel->id(), m_user->id(), command, DateTime::utc()));
  packet->setDirection(Notice::Server2Client);
  packet->setText(channel->name());
  packet->gender        = channel->gender().raw();
  packet->channelStatus = channel->status().value();

  if (channel->type() == SimpleID::ChannelId) {
    if (!forbidden) {
      packet->channels = channel->channels().all();
      if (Ch::server()->feed(FEED_NAME_SERVER)->data().value(SERVER_FEED_OFFLINE_KEY).toBool())
        packet->channels.append(channel->offline().all());
    }
    else
      packet->setStatus(Notice::Forbidden);
  }

  if (packet->status() == Notice::OK)
    packet->setData(channel->feeds().f(m_user.data()));

  return packet;
}


void NodeChannels::reply(int status)
{
  m_core->send(m_user->sockets(), ChannelNotice::reply(*m_packet, status));
}
