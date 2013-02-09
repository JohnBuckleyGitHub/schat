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

#include <QCoreApplication>
#include <QEvent>
#include <QThread>

#include "Account.h"
#include "Ch.h"
#include "cores/AnonymousAuth.h"
#include "cores/CookieAuth.h"
#include "cores/Core.h"
#include "cores/DiscoveryAuth.h"
#include "cores/ExternalAuth.h"
#include "DateTime.h"
#include "debugstream.h"
#include "events.h"
#include "feeds/ServerFeed.h"
#include "net/NodeAuthReply.h"
#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "net/packets/Notice.h"
#include "net/PacketWriter.h"
#include "net/Protocol.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "NodeNoticeReader.h"
#include "NodePlugins.h"
#include "sglobal.h"
#include "Sockets.h"
#include "Storage.h"

Core *Core::m_self = 0;

Core::Core(QObject *parent)
  : QObject(parent)
  , m_timestamp(0)
  , m_settings(Storage::settings())
  , m_storage(Storage::i())
{
  m_self = this;

  m_sendStream = new QDataStream(&m_sendBuffer, QIODevice::ReadWrite);
  m_readStream = new QDataStream(&m_readBuffer, QIODevice::ReadWrite);
}


Core::~Core()
{
  delete m_sendStream;
  delete m_readStream;
  qDeleteAll(m_auth);
}


/*!
 * Возвращает и при необходимости устанавливает отметку времени для следующего отправленного пакета.
 */
qint64 Core::date()
{
  if (m_self->m_timestamp == 0)
    m_self->m_timestamp = DateTime::utc();

  return m_self->m_timestamp;
}


/*!
 * Маршрутизация входящих пакетов.
 */
bool Core::route()
{
  return route(Ch::channel(m_reader->dest(), SimpleID::typeOf(m_reader->dest())));
}


/*!
 * Маршрутизация входящих пакетов.
 */
bool Core::route(ChatChannel channel)
{
  if (!channel)
    return false;

  QList<quint64> sockets;
  if (channel->type() == SimpleID::UserId)
    sockets = channel->sockets();
  else
    sockets = Sockets::channel(channel);

  Sockets::echoFilter(Ch::channel(m_reader->sender(), SimpleID::UserId), sockets, m_reader->is(Protocol::EnableEcho));
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

  for (int i = 0; i < m_listeners.size(); ++i) {
    NewPacketsEvent *event = new NewPacketsEvent(sockets, packets, userId);
    event->timestamp = m_timestamp;
    event->option = option;
    QCoreApplication::postEvent(m_listeners.at(i), event);
  }

  return true;
}


bool Core::send(const QList<quint64> &sockets, Packet packet, int option, const QByteArray &userId)
{
  return send(sockets, packet->data(m_sendStream), option, userId);
}


/*!
 * Отправка пакета текущему сокету.
 */
bool Core::send(const QByteArray &packet)
{
  return i()->send(QList<quint64>() << socket(), packet);
}


/*!
 * Отправка пакетов текущему сокету.
 */
bool Core::send(const QList<QByteArray> &packets)
{
  return i()->send(QList<quint64>() << socket(), packets);
}


/*!
 * Отправка пакета текущему сокету.
 */
bool Core::send(Packet packet)
{
  return i()->send(QList<quint64>() << socket(), packet->data(stream()));
}


bool Core::add(ChatChannel channel)
{
  if (Ch::add(channel)) {
    NodeNoticeReader::add(channel);
    return true;
  }

  return false;
}


/*!
 * Успешная авторизация пользователя.
 */
void Core::accept(const AuthResult &result, const QString &host)
{
  ChatChannel channel = Ch::channel(result.id);
  if (!channel)
    return;

  if (!channel->account()->saved)
    channel->createAccount();

  QList<QByteArray> packets;
  if (result.packet) {
    NodeAuthReply reply(result, channel);
    reply.host = host.isEmpty() ? packetsEvent()->address.toString() : host;
    packets.append(reply.data(m_sendStream));
  }

  NodeNoticeReader::accept(channel, result, packets);

  send(channel->sockets(), packets, result.option, channel->id());
}


/*!
 * Отклонение авторизации.
 *
 * \param result Информация с результатом авторизации.
 * \param socket Номер сокета, если 0, то он будет определён автоматически.
 */
void Core::reject(const AuthResult &result, quint64 socket)
{
  if (!socket)
    socket = m_socket;

  if (result.status != Notice::Found) {
    SCHAT_LOG_WARN_STR("auth rejected. status:\"" + QByteArray::number(result.status) + ' ' + Notice::status(result.status).toUtf8() + "\", socket:" + QByteArray::number(socket))
  }

  NodeAuthReply reply(result);

  for (int i = 0; i < m_listeners.size(); ++i) {
    NewPacketsEvent *event = new NewPacketsEvent(QList<quint64>() << socket, reply.data(m_sendStream));
    event->option = result.option;
    QCoreApplication::postEvent(m_listeners.at(i), event);
  }
}


void Core::customEvent(QEvent *event)
{
  switch ((int) event->type()) {
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
  m_socket = event->socket();

  while (!packets.isEmpty()) {
    m_readBuffer = packets.takeFirst();
    PacketReader reader(m_readStream);
    m_reader = &reader;

    if (reader.type() == Protocol::AuthRequestPacket) {
      auth();
      continue;
    }

    /// Идентификатор клиента не должен быть пустым или не верным.
    if (event->channelId().isEmpty() || Ch::channel(event->channelId()) == 0)
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

  if (m_auth.isEmpty()) {
    const QStringList methods = Ch::server()->feed(FEED_NAME_SERVER)->data().value(SERVER_FEED_AUTH_KEY).toStringList();
    if (methods.contains(AUTH_METHOD_ANONYMOUS))
      addAuth(new AnonymousAuth(this));

    addAuth(new CookieAuth(this));
    addAuth(new DiscoveryAuth(this));

    if (methods.contains(AUTH_METHOD_OAUTH))
      addAuth(new ExternalAuth(this));
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
 * Обработка физического отключения пользователя от сервера.
 */
void Core::release(SocketReleaseEvent *event)
{
  ChatChannel user = Ch::channel(event->channelId(), SimpleID::UserId);
  if (!user)
    return;

  user->hosts()->remove(event->socket());
  NodeNoticeReader::release(user, event->socket());

  Ch::gc(user);
}


/*!
 * Чтение пакетов типа Protocol::NoticePacket.
 * \sa Notice.
 */
void Core::notice(quint16 type)
{
  if (NodeNoticeReader::read(type, m_reader))
    route();
}
