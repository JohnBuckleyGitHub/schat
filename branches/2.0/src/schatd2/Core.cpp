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
#include "net/packets/message.h"
#include "net/packets/users.h"
#include "net/PacketWriter.h"
#include "net/Protocol.h"
#include "net/SimpleID.h"
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
  m_storage = new Storage(SimpleID::uniqueId());
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
 * Маршрутизация входящих пакетов.
 */
bool Core::route()
{
  QByteArray dest = m_reader->dest();
  if (dest.isEmpty())
    return false;

  int idType = SimpleID::typeOf(dest);

  if (idType == SimpleID::ChannelId) {
    return route(m_storage->channel(dest));
  }

  if (idType == SimpleID::UserId) {
    return route(m_storage->user(dest));
  }

  return false;
}


/*!
 * Маршрутизация входящего пакета адресом назначения, которого является канал.
 */
bool Core::route(ServerChannel *channel)
{
  if (!channel)
    return false;

  return send(channel, m_readBuffer);
}


/*!
 * Маршрутизация входящего пакета адресом назначения, которого является пользователь.
 */
bool Core::route(ServerUser *user)
{
  if (!user)
    return false;

  return send(user, m_readBuffer);
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

  SCHAT_DEBUG_STREAM(this << "newPacketsEvent()" << packets.size())

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

      int idType = SimpleID::typeOf(reader.dest());

      if (idType == SimpleID::InvalidId)
        continue;

      if (idType == SimpleID::UserId && m_storage->user(reader.dest()) == 0)
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

  MessageData message(user->id(), QByteArray(), "leave", "");
  MessageWriter leave(m_sendStream, message);
  send(m_storage->socketsFromUser(user), QList<QByteArray>() << leave.data());

  m_storage->remove(event->userId());
}


/*!
 * Подключение пользователя к каналу.
 *
 * \param userId  Идентификатор пользователя.
 * \param channel Указатель на канал.
 *
 * \return true в случае успеха.
 */
bool Core::join(const QByteArray &userId, const QByteArray &channelId)
{
  ServerChannel *channel = m_storage->channel(channelId);

  if (!channel)
    return false;

  return join(userId, channel);
}


/*!
 * Подключение пользователя к каналу.
 * Пользователь будет добавлен в канал, затем ему будет отослана информация
 * о канале.
 * В случае если в канале находятся другие пользователи, им будет разослано
 * уведомление о входе нового пользователя, после этого подключившемуся
 * пользователю будут отосланы данные всех пользователей в канале.
 *
 * \param userId  Идентификатор пользователя.
 * \param channel Указатель на канал.
 *
 * \return true в случае успеха.
 */
bool Core::join(const QByteArray &userId, ServerChannel *channel)
{
  if (!channel->addUser(userId))
    return false;

  ServerUser *user = m_storage->user(userId);
  if (!user)
    return false;

  user->addChannel(channel->id());

  ChannelWriter writer(m_sendStream, channel);
  send(user, writer.data());

  if (channel->userCount() > 1) {
    UserWriter writer(m_sendStream, user, channel->id());
    send(channel, writer.data());
    sendChannel(channel, user);
  }

  return true;
}


/*!
 * Возвращает канал, ассоциированный с именем \p name.
 * В случае если \p create true то канал будет создан в случае необходимости.
 *
 * \param name   Не нормализованное имя канала.
 * \param create true если канал не существует, то он будет создан.
 *
 * \return Указатель на канал или 0 в случае ошибки.
 */
ServerChannel *Core::channel(const QString &name, bool create)
{
  ServerChannel *channel = m_storage->channel(name, true);

  if (!channel && create) {
    channel = m_storage->addChannel(name);
  }

  return channel;
}


/*!
 * Обработка команд.
 *
 * \return true в случае если команда была обработана, иначе false.
 */
bool Core::command()
{
  SCHAT_DEBUG_STREAM(this << "command()" << m_packetsEvent->userId().toHex())

  QString command = m_messageData->command;

  if (command.isEmpty())
    return false;

  if (command == "join") {
    readJoinCmd();
    return true;
  }

  else if (command == "part") {
    if (m_storage->removeUserFromChannel(m_packetsEvent->userId(), m_reader->dest()))
      return false;
    else
      return true;
  }

  return false;
}


/*!
 * Чтение пакета Packet::AuthRequest.
 */
bool Core::readAuthRequest()
{
  int error = auth();

  if (error == AuthReplyData::NoError)
    return true;

  QByteArray packet = AuthReplyWriter(m_sendStream, AuthReplyData(m_storage->id(), error)).data();
  NewPacketsEvent *event = new NewPacketsEvent(m_packetsEvent->workerId(), m_packetsEvent->socketId(), packet);

  switch (error) {
    case AuthReplyData::AuthTypeNotImplemented:
    case AuthReplyData::UserIdAlreadyUse:
    case AuthReplyData::InvalidUser:
      event->option = NewPacketsEvent::KillSocketOption;
      break;

    default:
      break;
  }

  QCoreApplication::postEvent(m_workers.at(m_packetsEvent->workerId()), event);
  return false;
}


/*!
 * Обработка команды "join".
 */
bool Core::readJoinCmd()
{
  ServerChannel *chan = 0;
  if (!m_messageData->destId.isEmpty())
    chan = m_storage->channel(m_messageData->destId);

  if (!chan)
    chan = channel(m_messageData->text);

  if (!chan)
    return false;

  return join(m_packetsEvent->userId(), chan);
}


/*!
 * Чтение пакета Protocol::Message.
 */
bool Core::readMessage()
{
  MessageReader reader(m_reader);
  m_messageData = &reader.data;

  SCHAT_DEBUG_STREAM(this << "message()" << m_messageData->options);

  if (m_messageData->options & MessageData::ControlOption && command())
    return true;

  return route();
}


/*!
 * Процедура авторизации пользователя, чтение пакета Packet::AuthRequest.
 */
int Core::auth()
{
  AuthRequestData data = AuthRequestReader(m_reader).data;

  SCHAT_DEBUG_STREAM(this << "auth" << data.authType << data.host << data.nick << data.userAgent)

  if (data.authType != AuthRequestData::Anonymous)
    return AuthReplyData::AuthTypeNotImplemented;

  QByteArray userId = m_storage->makeUserId(data.authType, data.uniqueId);
  ServerUser *user = m_storage->user(userId);

  if (user)
    return AuthReplyData::UserIdAlreadyUse;

  QString normalNick = m_storage->normalize(data.nick);
  if (m_storage->user(normalNick, false))
    return AuthReplyData::NickAlreadyUse;

  user = new ServerUser(m_storage->session(), normalNick, userId, &data, m_packetsEvent->workerId(), m_packetsEvent->socketId());
  if (!user->isValid())
    return AuthReplyData::InvalidUser;

  m_storage->add(user);

  AuthReplyData reply(m_storage->id(), user->id(), user->session());
  send(user, AuthReplyWriter(m_sendStream, reply).data(), NewPacketsEvent::AuthorizeSocketOption);

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

  MessageData message(user->id(), channelId, "BSCh", "");
  MessageWriter begin(m_sendStream, message);
  packets.append(begin.data());

  for (int i = 0; i < users.size(); ++i) {
    ServerUser *u = m_storage->user(users.at(i));
    if (u) {
      packets.append(UserWriter(m_sendStream, u, channelId).data());
    }
  }

  message.command = "ESCh";
  MessageWriter end(m_sendStream, message);
  packets.append(end.data());

  send(user, packets);
}
