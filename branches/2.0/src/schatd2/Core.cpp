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
#include "net/packets/notices.h"
#include "net/packets/users.h"
#include "net/PacketWriter.h"
#include "net/Protocol.h"
#include "net/ServerData.h"
#include "net/SimpleID.h"
#include "ServerChannel.h"
#include "ServerUser.h"
#include "Storage.h"
#include "Worker.h"
#include "WorkerThread.h"

Core::Core(QObject *parent)
  : QObject(parent),
    m_thread(0)
{
  m_storage = new Storage();
  m_sendStream = new QDataStream(&m_sendBuffer, QIODevice::ReadWrite);
  m_readStream = new QDataStream(&m_readBuffer, QIODevice::ReadWrite);

  m_options.insert(MainChannelName, "Main");
  m_options.insert(ServerName, "");
  m_options.insert(PrivateId, SimpleID::uniqueId());
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
  m_storage->serverData()->setPrivateId(option(PrivateId).toByteArray());
  m_storage->serverData()->setName(option(ServerName).toString());

  if (!option(MainChannelName).toString().isEmpty()) {
    m_storage->serverData()->setChannelId(m_storage->addChannel(option(MainChannelName).toString(), true)->id());
  }

  m_thread = new WorkerThread(this);
  connect(m_thread, SIGNAL(workersStarted()), SLOT(workersStarted()));
  m_thread->start();

  return 0;
}


QVariant Core::option(Options key) const
{
  return m_options.value(key);
}


void Core::quit()
{
  m_thread->quit();
  m_thread->wait();
}


void Core::setOption(Options key, const QVariant &value)
{
  switch (key) {
    case ServerName:
      m_storage->serverData()->setName(value.toString());
      break;

    default:
      break;
  }

  m_options[key] = value;
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


bool Core::broadcast()
{
  ServerUser *user = m_storage->user(m_reader->sender());
  if (!user)
    return false;

  return send(echoFilter(m_storage->socketsFromUser(user)), m_readBuffer);
}


/*!
 * Маршрутизация входящих пакетов.
 */
bool Core::route()
{
  if (m_reader->headerOption() & Protocol::Broadcast)
    return broadcast();

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
 *
 * \param channel Канал назначения.
 */
bool Core::route(ServerChannel *channel)
{
  if (!channel)
    return false;

  QList<quint64> sockets = echoFilter(m_storage->socketsFromChannel(channel));
  if (sockets.isEmpty())
    return true;

  return send(sockets, m_readBuffer);
}


/*!
 * Маршрутизация входящего пакета адресом назначения, которого является пользователь.
 *
 * \param user Получатель сообщения.
 */
bool Core::route(ServerUser *user)
{
  if (!user)
    return false;

  bindTalks(m_storage->user(m_reader->sender()), user);

  return send(user, m_readBuffer);
}


bool Core::send(const QList<quint64> &sockets, const QByteArray &packet)
{
  return send(sockets, QList<QByteArray>() << packet);
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
  if (!user)
    return false;

  return send(user, QList<QByteArray>() << packet, option);
}


/*!
 * Отправка пакетов пользователю.
 */
bool Core::send(ServerUser *user, const QList<QByteArray> &packets, int option)
{
  if (!user)
    return false;

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
    if (!reader.sender().isEmpty()) {
      if (event->userId() != reader.sender())
        continue;

      if (!(reader.headerOption() & Protocol::Broadcast)) {
        int idType = SimpleID::typeOf(reader.dest());

        if (idType == SimpleID::InvalidId)
          continue;

        if (m_reader->type() != Protocol::MessagePacket && idType == SimpleID::UserId && m_storage->user(reader.dest()) == 0)
          continue;
      }
    }

    switch (reader.type()) {
      case Protocol::MessagePacket:
        readMessage();
        break;

      case Protocol::UserDataPacket:
        readUserData();
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

  QByteArray id = user->id();

  QList<QByteArray> talks = user->ids(SimpleID::TalksListId);
  for (int i = 0; i < talks.size(); ++i) {
    User *u = m_storage->user(talks.at(i));
    if (u) {
      u->removeId(SimpleID::TalksListId, id);
    }
  }

  MessageData message(user->id(), QByteArray(), "leave", "");
  MessageWriter leave(m_sendStream, message);
  send(m_storage->socketsFromUser(user), QList<QByteArray>() << leave.data());

  m_storage->remove(event->userId());
}


/*!
 * Подключение пользователя к каналу.
 *
 * \param userId    Идентификатор пользователя.
 * \param channelId Идентификатор канала.
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

  user->addId(SimpleID::ChannelListId, channel->id());

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
 * Фильтрует список сокетов, в зависимости от необходимости эха.
 */
QList<quint64> Core::echoFilter(const QList<quint64> &sockets)
{
  QList<quint64> out = sockets;
  ServerUser *user = m_storage->user(m_reader->sender());

  if (user) {
    quint64 id = user->socketId();
    if (m_reader->headerOption() & Protocol::EnableEcho) {
      if (!out.contains(id)) {
        out.append(id);
      }
    }
    else {
      out.removeAll(id);
    }
  }

  return out;
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
 * Чтение пакета Packet::AuthRequest.
 */
bool Core::readAuthRequest()
{
  int error = auth();

  if (error == AuthReplyData::NoError)
    return true;

  QByteArray packet = AuthReplyWriter(m_sendStream, AuthReplyData(m_storage->serverData(), error)).data();
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


bool Core::readUserData()
{
  UserReader reader(m_reader);

  SCHAT_DEBUG_STREAM(this << "readUserData()" << reader.user.nick())

  ServerUser *user = m_storage->user(m_reader->sender());
  if (!user)
    return false;

  if (user->nick() != reader.user.nick()) {
    if (m_storage->user(reader.user.nick(), true))
      return false;
  }

  user->setNick(reader.user.nick());
  user->setRawGender(reader.user.rawGender());
  m_storage->rename(user);

  route();
  return true;
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

  AuthReplyData reply(m_storage->serverData(), user->id(), user->session());
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
 * \todo ! Не отправлять клиенту информацию о тех пользователях информация о которых имеется у клиента.
 */
void Core::sendChannel(ServerChannel *channel, ServerUser *user)
{
  QList<QByteArray> users = channel->users();
  users.removeAll(user->id());
  user->addUsers(users);

  QByteArray channelId = channel->id();
  QList<QByteArray> packets;

  MessageData message(user->id(), channelId, "BSCh", "");
  MessageWriter begin(m_sendStream, message);
  packets.append(begin.data());

  for (int i = 0; i < users.size(); ++i) {
    ServerUser *u = m_storage->user(users.at(i));
    if (u) {
      u->addUser(user->id());
      packets.append(UserWriter(m_sendStream, u, channelId).data());
    }
  }

  message.command = "ESCh";
  MessageWriter end(m_sendStream, message);
  packets.append(end.data());

  send(user, packets);
}


/*!
 * При необходимости добавляет пользователя \p user2 в список разговоров пользователя \p user1.
 * В случае успешного добавления пользователю \p user1 будет отослан пакет с данными \p user2.
 */
void Core::addTalk(ServerUser *user1, ServerUser *user2)
{
  if (!user1->containsId(SimpleID::TalksListId, user2->id())) {
    if (user1->addId(SimpleID::TalksListId, user2->id())) {
      send(user1, UserWriter(m_sendStream, user2, user1->id()).data());
    }
  }
}


/*!
 * Синхронизация данных для обмена статусной информацией между пользователями.
 * \todo ! Добавить возможность склейки пакетов.
 */
void Core::bindTalks()
{
  SCHAT_DEBUG_STREAM(this << "addUserRoleId()")

  ServerUser *user = m_storage->user(m_reader->sender());
  if (!user)
    return;

  QList<QByteArray> ids = m_reader->idList();
  if (ids.isEmpty())
    return;

  ServerUser *destUser = 0;

  for (int i = 0; i < ids.size(); ++i) {
    destUser = m_storage->user(ids.at(i));
    if (!destUser)
      continue;

    bindTalks(user, destUser);
  }
}


/*!
 * Взаимное добавление пользователей в список разговоров.
 * \sa addTalk().
 */
void Core::bindTalks(ServerUser *senderUser, ServerUser *destUser)
{
  if (!senderUser || !destUser)
    return;

  SCHAT_DEBUG_STREAM(this << "bindTalks()")

  addTalk(senderUser, destUser);
  addTalk(destUser, senderUser);
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

  if (command == "part") {
    if (m_storage->removeUserFromChannel(m_packetsEvent->userId(), m_reader->dest()))
      return false;
    else
      return true;
  }

  if (command == "add") {
    if (SimpleID::isUserRoleId(m_reader->sender(), m_reader->dest()) && SimpleID::typeOf(m_reader->dest()) == SimpleID::TalksListId) {
      bindTalks();
    }

    return true;
  }

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
 * Чтение пакета Protocol::MessagePacket.
 */
bool Core::readMessage()
{
  MessageReader reader(m_reader);
  m_messageData = &reader.data;

  SCHAT_DEBUG_STREAM(this << "message()" << m_messageData->options);

  if (SimpleID::typeOf(m_reader->dest()) == SimpleID::UserId && m_storage->user(m_reader->dest()) == 0) {
    rejectMessage(NoticeData::UserUnavailable);
    return false;
  }

  if (m_messageData->options & MessageData::ControlOption && command())
    return true;

  if (m_messageData->name == 0)
    return route();

  if (!route()) {
    rejectMessage(NoticeData::UnknownError);
    return false;
  }

  return true;
}


void Core::rejectMessage(int reason)
{
  SCHAT_DEBUG_STREAM("rejectMessage()" << reason)

  if (m_messageData->name == 0)
    return;

  NoticeData data(m_messageData->senderId, m_messageData->destId, NoticeData::MessageRejected, m_messageData->name, reason);
  send(m_storage->user(m_messageData->senderId), NoticeWriter(m_sendStream, data).data());
}
