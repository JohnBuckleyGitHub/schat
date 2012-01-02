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

#include <QCoreApplication>
#include <QEvent>
#include <QThread>

#include "Account.h"
#include "cores/AnonymousAuth.h"
#include "cores/CookieAuth.h"
#include "cores/Core.h"
#include "cores/PasswordAuth.h"
#include "DateTime.h"
#include "debugstream.h"
#include "events.h"
#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "net/packets/ChannelPacket.h"
#include "net/packets/Notice.h"
#include "net/PacketWriter.h"
#include "net/Protocol.h"
#include "net/ServerData.h"
#include "net/SimpleID.h"
#include "NodeChannels.h"
#include "NodeFeeds.h"
#include "NodeLog.h"
#include "NodeMessages.h"
#include "NodePlugins.h"
#include "plugins/NodeHooks.h"
#include "Sockets.h"
#include "Storage.h"

Core::Core(QObject *parent)
  : QObject(parent)
  , m_plugins(0)
  , m_timestamp(0)
  , m_listener(0)
  , m_settings(Storage::i()->settings())
  , m_storage(Storage::i())
{
  addAuth(new AnonymousAuth(this));
  addAuth(new CookieAuth(this));

  if (Storage::i()->serverData()->is(ServerData::PasswordAuthSupport))
    addAuth(new PasswordAuth(this));

  m_sendStream = new QDataStream(&m_sendBuffer, QIODevice::ReadWrite);
  m_readStream = new QDataStream(&m_readBuffer, QIODevice::ReadWrite);

  new NodeChannels(this);
  new NodeMessages(this);
  new NodeFeeds(this);
}


Core::~Core()
{
  delete m_sendStream;
  delete m_readStream;
  qDeleteAll(m_auth);
}


/*!
 * Маршрутизация входящих пакетов.
 */
bool Core::route()
{
  if (m_timestamp == 0)
    m_timestamp = DateTime::utc();

  ChatChannel channel = m_storage->channel(m_reader->dest(), SimpleID::typeOf(m_reader->dest()));
  if (!channel)
    return false;

  QList<quint64> sockets;
  if (channel->type() == SimpleID::UserId)
    sockets = channel->sockets();
  else
    sockets = Sockets::channel(channel);

  Sockets::echoFilter(m_storage->channel(m_reader->sender(), SimpleID::UserId), sockets, m_reader->is(Protocol::EnableEcho));
  return send(sockets, m_readBuffer);
}


/*!
 * Отправка пакета списку сокетов.
 */
bool Core::send(const QList<quint64> &sockets, const QByteArray &packet, int option, const QByteArray &userId)
{
  return send(sockets, QList<QByteArray>() << packet, option, userId);
}


/*!
 * Базовая функция отправки пакетов.
 * Эта функция используется всеми остальными функциями отправки пакетов.
 *
 * \param sockets Список номеров сокетов.
 * \param packets Готовые сформированные пакеты.
 * \param option  Опция сокета NewPacketsEvent::Option.
 * \param userId  Идентификатор пользователя.
 */
bool Core::send(const QList<quint64> &sockets, const QList<QByteArray> &packets, int option, const QByteArray &userId)
{
  if (sockets.isEmpty())
    return true;

  if (m_timestamp == 0)
    m_timestamp = DateTime::utc();

  NewPacketsEvent *event = new NewPacketsEvent(sockets, packets, userId);
  event->timestamp = m_timestamp;
  event->option = option;

  QCoreApplication::postEvent(m_listener, event);
  return true;
}


QByteArray Core::id() const
{
  return m_storage->serverData()->id();
}


bool Core::add(ChatChannel channel, int authType, const QByteArray &authId)
{
  Q_UNUSED(authType);
  Q_UNUSED(authId)

  if (m_storage->add(channel)) {
    NodeNoticeReader::add(channel);
    return true;
  }

  return false;
}


void Core::customEvent(QEvent *event)
{
  switch (event->type()) {
    case ServerEvent::NewPackets:
      newPacketsEvent(static_cast<NewPacketsEvent*>(event));
      break;

    case ServerEvent::SocketRelease:
      release(static_cast<SocketReleaseEvent*>(event));
      break;

    default:
      break;
  }
}


bool Core::checkPacket()
{
  if (m_storage->isAllowSlaves() && m_storage->isSlave(m_packetsEvent->channelId()))
    return true;

  if (m_reader->sender().isEmpty())
    return false;

  if (m_packetsEvent->channelId() != m_reader->sender())
    return false;

  return true;
}


/*!
 * Обработка новых пакетов.
 */
void Core::newPacketsEvent(NewPacketsEvent *event)
{
  m_packetsEvent = event;
  QList<QByteArray> packets = event->packets;

  while (!packets.isEmpty()) {
    m_readBuffer = packets.takeFirst();
    PacketReader reader(m_readStream);
    m_reader = &reader;

    if (reader.type() == Protocol::AuthRequestPacket) {
      auth();
      continue;
    }

    /// Идентификатор клиента не должен быть пустым или не верным.
    if (event->channelId().isEmpty() || m_storage->channel(event->channelId()) == 0)
      continue;

    if (!checkPacket())
      continue;

    m_timestamp = 0;
    packet(reader.type());
  }
}


void Core::packet(int type)
{
  switch (type) {
    case Protocol::NoticePacket:
      notice(m_reader->get<quint16>());
      break;

    default:
      route();
      break;
  }
}


/*!
 * Обработка авторизации пользователя.
 */
bool Core::auth()
{
  AuthRequest data(m_reader);

  if (!data.isValid()) {
    AuthResult result(Notice::BadRequest, data.id, NewPacketsEvent::KillSocketOption);
    reject(result);
    return false;
  }

  for (int i = 0; i < m_auth.size(); ++i) {
    if (data.authType != m_auth.at(i)->type())
      continue;

    AuthResult result = m_auth.at(i)->auth(data);
    if (result.action == AuthResult::Reject) {
      reject(result);
      return false;
    }
    else if (result.action == AuthResult::Accept) {
      accept(result);
      return true;
    }
    else if (result.action == AuthResult::Pending)
      return true;
  }

  AuthResult result(Notice::NotImplemented, data.id, NewPacketsEvent::KillSocketOption);
  reject(result);
  return false;
}


/*!
 * Успешная авторизация пользователя.
 */
void Core::accept(const AuthResult &result)
{
  ChatChannel channel = m_storage->channel(result.id);
  if (!channel)
    return;

  if (!channel->account())
    channel->createAccount();

  QList<QByteArray> packets;
  if (result.packet) {
    AuthReply reply(Storage::serverData(), channel.data(), channel->account()->cookie(), result.authId, result.json);
    packets.append(reply.data(m_sendStream));
  }

  packets.append(ChannelPacket::channel(channel, channel->id(), m_sendStream));

  send(channel->sockets(), packets, result.option, channel->id());
}


/*!
 * Отклонение авторизации.
 */
void Core::reject(const AuthResult &result)
{
  SCHAT_LOG_DEBUG(<< "REJECT AUTH" << result.status << Notice::status(result.status) << SimpleID::encode(result.authId));

  AuthReply reply(Storage::serverData(), result.status, result.authId, result.json);

  NewPacketsEvent *event = new NewPacketsEvent(QList<quint64>() << m_packetsEvent->socket(), reply.data(m_sendStream));
  event->option = result.option;

  QCoreApplication::postEvent(m_listener, event);
}


/*!
 * Обработка физического отключения пользователя от сервера.
 */
void Core::release(SocketReleaseEvent *event)
{
  ChatChannel user = m_storage->channel(event->channelId(), SimpleID::UserId);
  if (!user)
    return;

  user->removeSocket(event->socket());

  NodeNoticeReader::release(user, event->socket());

  m_storage->gc(user);
}


/*!
 * Чтение пакетов типа Protocol::NoticePacket.
 * \sa Notice.
 */
void Core::notice(quint16 type)
{
  if (type == Notice::GenericType) {
    Notice notice(type, m_reader);
    m_notice = &notice;


    QString command = m_notice->command();
    SCHAT_LOG_TRACE(<< "NOTICE PACKET" << command << notice.text() << notice.raw());


//    else if (command == "leave")
//      leave(m_storage->channel(m_reader->sender()), m_packetsEvent->socket());

  }
  else {
    if (NodeNoticeReader::read(type, m_reader))
      route();

    return;
  }

  route();
}
