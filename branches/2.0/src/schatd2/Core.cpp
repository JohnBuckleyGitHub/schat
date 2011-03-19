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

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QEvent>
#include <QThread>

#include "Core.h"
#include "debugstream.h"
#include "events.h"
#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "net/packets/channels.h"
#include "net/packets/users.h"
#include "net/PacketWriter.h"
#include "net/Protocol.h"
#include "ServerChannel.h"
#include "ServerUser.h"
#include "Storage.h"
#include "Worker.h"
#include "WorkerThread.h"

#define SCHAT_EVENT(x, y) case ServerEvent:: x: y (static_cast< xEvent *>(event)); break;

Core::Core(QObject *parent)
  : QObject(parent),
    m_thread(0)
{
  m_storage = new Storage(AuthRequest::genUniqueId());
  m_storage->addChannel("Main", true);
  m_sendStream = new QDataStream(&m_sendBuffer, QIODevice::ReadWrite);
  m_readStream = new QDataStream(&m_readBuffer, QIODevice::ReadWrite);
}


Core::~Core()
{
  delete m_storage;
  delete m_sendStream;
  delete m_readStream;
  delete m_thread;
}


int Core::start()
{
  m_thread = new WorkerThread(this);
  connect(m_thread, SIGNAL(workersStarted()), SLOT(workersStarted()));
  m_thread->start();

  return 0;
}


void Core::quit()
{
  m_thread->quit();
  m_thread->wait();
}


void Core::customEvent(QEvent *event)
{
  SCHAT_DEBUG_STREAM(this << "customEvent()" << event->type())

  switch (event->type()) {
    case ServerEvent::NewPackets:
      newPacketsEvent(static_cast<NewPacketsEvent*>(event));
      break;

    case ServerEvent::SocketRelease:
      socketReleaseEvent(static_cast<SocketReleaseEvent*>(event));
      break;

    default:
      break;
  }
}


void Core::workersStarted()
{
  m_workers = m_thread->workers();
}


/*!
 * Перенаправление полученного пакета в соответствии с адресом назначения.
 */
bool Core::route()
{
  QByteArray dest = m_reader->dest();
  if (dest.isEmpty())
    return false;

  int idType = Packet::idType(dest);

  if (idType == Protocol::ChannelId) {
    ServerChannel *chan = m_storage->channel(dest);
    if (!chan)
      return false;

    send(chan, m_readBuffer);
    return true;
  }
  else if (idType == Protocol::UserId) {
    ServerUser *user = m_storage->user(dest);
    if (!user)
      return false;

    send(user, m_readBuffer);
    return true;
  }

  return false;
}


bool Core::send(const QList<quint64> &sockets, const QList<QByteArray> &packets)
{
  if (sockets.isEmpty())
    return false;

  NewPacketsEvent *event = new NewPacketsEvent(sockets, packets);
  QCoreApplication::postEvent(m_workers.at(0), event);
  return true;
}


/*!
 * Отправка пакета всем пользователям в канале.
 * \param channel Канал.
 * \param packet  Пакет.
 * \return false в случае ошибки, иначе true.
 */
bool Core::send(ServerChannel *channel, const QByteArray &packet)
{
  return send(m_storage->socketsFromChannel(channel), QList<QByteArray>() << packet);
}


/*!
 * Отправка пакетов всем пользователям в канале.
 * \param channel Канал.
 * \param packets Пакеты.
 * \return false в случае ошибки, иначе true.
 */
bool Core::send(ServerChannel *channel, const QList<QByteArray> &packets)
{
  return send(m_storage->socketsFromChannel(channel), packets);
}


/*!
 * Отправка пакетов пользователю.
 */
bool Core::send(ServerUser *user, const QByteArray &packet, int option)
{
  return send(user, QList<QByteArray>() << packet, option);
}


/*!
 * Отправка пакетов пользователю.
 */
bool Core::send(ServerUser *user, const QList<QByteArray> &packets, int option)
{
  NewPacketsEvent *event = new NewPacketsEvent(user->workerId(), user->socketId(), packets, user->id());
  event->option = option;
  QCoreApplication::postEvent(m_workers.at(user->workerId()), event);
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
      readAuthRequest();
      return;
    }

    /// Идентификатор клиента не должен быть пустым или не верным.
    if (event->userId().isEmpty() || m_storage->user(event->userId()) == 0)
      continue;

    /// В случае если пакет содержит информацию об отправителе и получателе,
    /// идентификатор отправителя не должен быть поддельным и получатель
    /// должен быть известен серверу.
    if (!reader.isBasic()) {
      if (event->userId() != reader.sender())
        continue;

      int idType = Packet::idType(reader.dest());

      if (idType == Protocol::InvalidId)
        continue;

      if (idType == Protocol::ChannelId && m_storage->channel(reader.dest()) == 0)
        continue;

      if (idType == Protocol::UserId && m_storage->user(reader.dest()) == 0)
        continue;
    }

    switch (reader.type()) {
      case Protocol::MessagePacket:
        readMessage();
        break;

      case Protocol::UserDataPacket:
        break;

      default:
        route();
        break;
    }
  }
}


/*!
 * Обработка отключения пользователя.
 */
void Core::socketReleaseEvent(SocketReleaseEvent *event)
{
  ServerUser *user = m_storage->user(event->userId());
  if (!user)
    return;

  PacketWriter leave(m_sendStream, Protocol::MessagePacket, user->id());
  leave.put<quint8>(Packet::ControlMessage);
  leave.put("leave");
  send(m_storage->socketsFromUser(user), QList<QByteArray>() << leave.data());

  m_storage->remove(event->userId());
}


/*!
 * Обработка пакета Protocol::Message с опцией Packet::ControlMessage.
 */
bool Core::command()
{
  SCHAT_DEBUG_STREAM(this << "command()" << m_packetsEvent->userId().toHex())

  QString command = m_reader->text();
  if (command.isEmpty())
    return false;

  if (command == "join") {
    readJoinCmd();
  }
  else if (command == "part") {
    if (m_storage->removeUserFromChannel(m_packetsEvent->userId(), m_reader->dest()))
      route();
  }
  else
    route();

  return true;
}


/*!
 * Чтение пакета Packet::AuthRequest.
 */
bool Core::readAuthRequest()
{
  AuthReply::Error error = static_cast<AuthReply::Error>(auth());
  if (error == AuthReply::NoError)
    return true;

  AuthReply reply(error);
  NewPacketsEvent *e = new NewPacketsEvent(m_packetsEvent->workerId(), m_packetsEvent->socketId(), reply.data(m_sendStream));

  switch (error) {
    case AuthReply::AuthTypeNotImplemented:
    case AuthReply::UserIdAlreadyUse:
    case AuthReply::InvalidUser:
      e->option = NewPacketsEvent::KillSocketOption;
      break;

    default:
      break;
  }

  QCoreApplication::postEvent(m_workers.at(m_packetsEvent->workerId()), e);
  return false;
}


bool Core::readJoinCmd()
{
  QString param = m_reader->text();
  bool newChannel = false;
  ServerChannel *channel = m_storage->channel(param, true);

  if (!channel) {
    channel = m_storage->addChannel(param);
    newChannel = true;
  }

  if (!channel)
    return false;

  if (!channel->addUser(m_packetsEvent->userId()))
    return false;

  ServerUser *user = m_storage->user(m_packetsEvent->userId());
  if (!user)
    return false;

  user->addChannel(channel->id());

  // Отправка пользователю информации о канале.
  JoinReply reply(channel);
  send(user, reply.data(m_sendStream));

  // Отправка уведомления всем пользователям в канале о входе нового пользователя.
  // Отправка пользователю всех пользователей в канале.
  if (!newChannel && channel->userCount() > 1) {
    UserData userData(user, channel->id());
    send(channel, userData.data(m_sendStream));

    sendChannel(channel, user);
  }

  return true;
}


/*!
 * Чтение пакета Protocol::Message.
 */
bool Core::readMessage()
{
  int options = m_reader->get<quint8>();

  SCHAT_DEBUG_STREAM(this << "message()" << options);

  if (options == Packet::ControlMessage)
    return command();

  return route();
}


/*!
 * Процедура авторизации пользователя, чтение пакета Packet::AuthRequest.
 */
int Core::auth()
{
  AuthRequest authRequest(m_reader);

  SCHAT_DEBUG_STREAM(this << "auth" << authRequest.authType() << authRequest.host() << authRequest.nick() << authRequest.userAgent())

  if (authRequest.authType() != AuthRequest::Anonymous)
    return AuthReply::AuthTypeNotImplemented;

  QByteArray userId = m_storage->makeUserId(authRequest.authType(), authRequest.uniqueId());
  ServerUser *user = m_storage->user(userId);

  if (user)
    return AuthReply::UserIdAlreadyUse;

  QString normalNick = m_storage->normalize(authRequest.nick());
  if (m_storage->user(normalNick, false))
    return AuthReply::NickAlreadyUse;

  user = new ServerUser(m_storage->session(), normalNick, userId, &authRequest, m_packetsEvent->workerId(), m_packetsEvent->socketId());
  if (!user->isValid())
    return AuthReply::InvalidUser;

  m_storage->add(user);
  AuthReply reply(m_storage->id(), user->id(), user->session());
  send(user, reply.data(m_sendStream), NewPacketsEvent::AuthorizeSocketOption);
  return 0;
}


/*!
 * Отправка данных пользователей в канале.
 * - Первым пакетом будет команда "BSCh" (Begin Sync Channel).
 * - Затем идут данные пользователей в виде пакетов UserData.
 * - Завершающий пакет, это команда "ESCh" (End Sync Channel).
 *
 * \param channel Канал, из которого будут отправлены данные пользователей.
 * \param user    Пользователь, которому будут отравлены данные.
 */
void Core::sendChannel(ServerChannel *channel, ServerUser *user)
{
  QList<QByteArray> users = channel->users();
  users.removeAll(user->id());

  QByteArray channelId = channel->id();
  QList<QByteArray> packets;

  PacketWriter begin(m_sendStream, Protocol::MessagePacket, user->id(), channelId);
  begin.put<quint8>(Packet::ControlMessage);
  begin.put("BSCh");
  packets.append(begin.data());

  for (int i = 0; i < users.size(); ++i) {
    ServerUser *u = m_storage->user(users.at(i));
    if (u) {
      packets.append(UserData(u, channelId).data(m_sendStream));
    }
  }

  PacketWriter end(m_sendStream, Protocol::MessagePacket, user->id(), channelId);
  end.put<quint8>(Packet::ControlMessage);
  end.put("ESCh");
  packets.append(end.data());

  send(user, packets);
}
