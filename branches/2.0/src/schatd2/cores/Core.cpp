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

#include "cores/AnonymousAuth.h"
#include "cores/Core.h"
#include "cores/NodeAuth.h"
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
#include "ServerUser.h"
#include "Storage.h"

Core::Core(QObject *parent)
  : QObject(parent)
  , m_timestamp(0)
  , m_listener(0)
  , m_settings(Storage::i()->settings())
  , m_storage(Storage::i())
{
  addAuth(new AnonymousAuth(this));

  m_sendStream = new QDataStream(&m_sendBuffer, QIODevice::WriteOnly);
  m_readStream = new QDataStream(&m_readBuffer, QIODevice::ReadOnly);
}


Core::~Core()
{
  delete m_sendStream;
  delete m_readStream;
  qDeleteAll(m_auth);
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


/*!
 * Маршрутизация входящих пакетов.
 */
bool Core::route()
{
  if (m_timestamp == 0)
    m_timestamp = timestamp();

  return send(echoFilter(m_storage->socketsFromIds(m_reader->destinations())), m_readBuffer);
}


/*!
 * Отправка пакета всем пользователям в канале.
 *
 * \param channel Канал.
 * \param packet  Пакет.
 * \return false в случае ошибки.
 */
bool Core::send(ChatChannel channel, const QByteArray &packet)
{
  return send(m_storage->socketsFromChannel(channel), QList<QByteArray>() << packet);
}


/*!
 * Отправка пакетов всем пользователям в канале.
 * \param channel Канал.
 * \param packets Пакеты.
 * \return false в случае ошибки, иначе true.
 */
bool Core::send(ChatChannel channel, const QList<QByteArray> &packets)
{
  return send(m_storage->socketsFromChannel(channel), packets);
}


/*!
 * Отправка пакета списку сокетов.
 */
bool Core::send(const QList<quint64> &sockets, const QByteArray &packet)
{
  return send(sockets, QList<QByteArray>() << packet);
}


/*!
 * Отправка пакетов списку сокетов.
 */
bool Core::send(const QList<quint64> &sockets, const QList<QByteArray> &packets)
{
  if (sockets.isEmpty())
    return true;

  if (m_timestamp == 0)
    m_timestamp = timestamp();

  NewPacketsEvent *event = new NewPacketsEvent(sockets, packets);
  event->timestamp = m_timestamp;
  QCoreApplication::postEvent(m_listener, event);
  return true;
}


/*!
 * Отправка пакетов пользователю.
 */
bool Core::send(ChatUser user, const QByteArray &packet, int option)
{
  if (!user)
    return false;

  return send(user, QList<QByteArray>() << packet, option);
}


/*!
 * Отправка пакетов пользователю.
 */
bool Core::send(ChatUser user, const QList<QByteArray> &packets, int option)
{
  if (!user)
    return false;

  if (m_timestamp == 0)
    m_timestamp = timestamp();

  NewPacketsEvent *event = new NewPacketsEvent(user->socketId(), packets, user->id());
  event->option = option;
  event->timestamp = m_timestamp;
  QCoreApplication::postEvent(m_listener, event);
  return true;
}


qint64 Core::timestamp() const
{
  #if QT_VERSION >= 0x040700
  return QDateTime::currentDateTimeUtc().toMSecsSinceEpoch();
  #else
  return qint64(QDateTime::currentDateTime().toUTC().toTime_t()) * 1000;
  #endif
}


bool Core::checkPacket()
{
  if (m_storage->isAllowSlaves() && m_storage->isSlave(m_packetsEvent->userId()))
    return true;

  if (m_reader->sender().isEmpty())
    return false;

  if (m_packetsEvent->userId() != m_reader->sender())
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

  SCHAT_DEBUG_STREAM(this << "newPacketsEvent()" << packets.size())

  while (!packets.isEmpty()) {
    m_readBuffer = packets.takeFirst();
    PacketReader reader(m_readStream);
    m_reader = &reader;

    if (reader.type() == Protocol::AuthRequestPacket) {
      readAuthRequest();
      continue;
    }

    /// Идентификатор клиента не должен быть пустым или не верным.
    if (event->userId().isEmpty() || m_storage->user(event->userId()) == 0)
      continue;

    if (!checkPacket())
      continue;

    m_timestamp = 0;
    readPacket(reader.type());
  }
}


void Core::readPacket(int type)
{
  qDebug() << "Core::readPacket()" << type;

  switch (type) {
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


/*!
 * Обработка отключения пользователя.
 */
void Core::socketReleaseEvent(SocketReleaseEvent *event)
{
  ChatUser user = m_storage->user(event->userId());
  if (!user)
    return;

  QByteArray id = user->id();
  MessageData message(id, user->channels(), QLatin1String("leave"), QString());
  MessageWriter leave(m_sendStream, message);

  send(m_storage->socketsFromIds(user->channels()), QList<QByteArray>() << leave.data());
  m_storage->remove(user);
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
  ChatChannel channel = m_storage->channel(channelId);

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
bool Core::join(const QByteArray &userId, ChatChannel channel)
{
  if (!channel->addUser(userId))
    return false;

  ChatUser user = m_storage->user(userId);
  if (!user)
    return false;

  user->addChannel(channel->id());

  ChannelWriter writer(m_sendStream, channel.data(), user->id());
  send(user, writer.data());

  if (channel->userCount() > 1) {
    UserWriter writer(m_sendStream, user.data(), channel->id());
    send(channel, writer.data()); // Отправка всем пользователям в канале данных нового пользователя.
    sendChannel(channel, user);
  }

  return true;
}


ChatChannel Core::addChannel(ChatUser user)
{
  ChatChannel channel = m_storage->addChannel(user);
  if (channel->userCount() > 1) {
    UserWriter writer(m_sendStream, user.data(), channel->id());
    send(channel, writer.data());
  }

  return channel;
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
ChatChannel Core::channel(const QString &name, bool create)
{
  if (create && name.startsWith(QLatin1String("~")))
    create = false;

  ChatChannel channel = m_storage->channel(name, true);

  if (!channel && create) {
    channel = m_storage->addChannel(name);
  }

  return channel;
}


/*!
 * Фильтрует список сокетов, в зависимости от необходимости эха.
 */
QList<quint64> Core::echoFilter(const QList<quint64> &sockets)
{
  QList<quint64> out = sockets;
  ChatUser user = m_storage->user(m_reader->sender());

  if (!user)
    return out;

  quint64 id = user->socketId();
  if (m_reader->is(Protocol::EnableEcho)) {
    if (!out.contains(id))
      out.append(id);
  }
  else {
    out.removeAll(id);
  }

  return out;
}


/*!
 * Чтение пакета Protocol::AuthRequestPacket.
 */
bool Core::readAuthRequest()
{
  qDebug() << "Core::readAuthRequest()";

  AuthRequestData data = AuthRequestReader(m_reader).data;
  for (int i = 0; i < m_auth.size(); ++i) {
    if (data.authType != m_auth.at(i)->type())
      continue;

    AuthResult result = m_auth.at(i)->auth(data);
    if (result.action == AuthResult::Reject) {
      rejectAuth(result);
      return false;
    }
    else if (result.action == AuthResult::Accept) {
      acceptAuth(result);
      return true;
    }
    else if (result.action == AuthResult::Pending)
      return true;
  }

  AuthResult result(AuthReplyData::AuthTypeNotImplemented);
  rejectAuth(result);

  return false;
}


/*!
 * Успешная авторизация пользователя.
 */
void Core::acceptAuth(const AuthResult &result)
{
  ChatUser user = m_storage->user(result.id);
  if (!user)
    return;

  addChannel(user);

  AuthReplyData reply(m_storage->serverData(), user.data());
  send(user, AuthReplyWriter(m_sendStream, reply).data(), result.option);
}


/*!
 * Отклонение авторизации.
 */
void Core::rejectAuth(const AuthResult &result)
{
  QByteArray packet = AuthReplyWriter(m_sendStream, AuthReplyData(m_storage->serverData(), result.error)).data();
  NewPacketsEvent *event = new NewPacketsEvent(m_packetsEvent->socket(), packet);
  event->option = result.option;

  QCoreApplication::postEvent(m_listener, event);
}


bool Core::readUserData()
{
  UserReader reader(m_reader);

  SCHAT_DEBUG_STREAM(this << "readUserData()" << reader.user.nick())

  ChatUser user = m_storage->user(m_reader->sender());
  if (!user)
    return false;

  if (user->nick() != reader.user.nick()) {
    ChatUser u = m_storage->user(reader.user.nick(), true);
    if (u && u != user)
      return false;
  }

  user->setNick(reader.user.nick());
  user->setRawGender(reader.user.rawGender());
  m_storage->rename(user);

  route();
  return true;
}


/*!
 * Отправка данных пользователей в канале.
 * - Данные пользователей в виде пакетов UserData.
 * - Завершающий пакет, это команда "synced".
 *
 * \param channel Канал, из которого будут отправлены данные пользователей.
 * \param user    Пользователь, которому будут отравлены данные.
 */
void Core::sendChannel(ChatChannel channel, ChatUser user)
{
  QList<QByteArray> users = channel->users();
  users.removeAll(user->id());

  QByteArray channelId = channel->id();
  QList<QByteArray> packets;

  for (int i = 0; i < users.size(); ++i) {
    ChatUser u = m_storage->user(users.at(i));
    if (!u)
      continue;

    u->addUser(user->id());
    if (!user->isUser(u->id()))
      packets.append(UserWriter(m_sendStream, u.data(), user->id(), channelId).data());
  }

  user->addUsers(users);
  MessageData message(channelId, user->id(), QLatin1String("synced"), QString());
  packets.append(MessageWriter(m_sendStream, message).data());

  send(user, packets);
}


/*!
 * Обновление статуса пользователя.
 */
bool Core::updateUserStatus()
{
  ChatUser user = m_storage->user(m_reader->sender());
  if (!user)
    return true;

  if (!user->setStatus(m_messageData->text))
    return true;

  if (user->status() == User::OfflineStatus)
    user->setStatus(User::OnlineStatus);

  return false;
}


/*!
 * Обработка команд.
 *
 * \return true в случае если команда была обработана, иначе false.
 */
bool Core::command()
{
  QString command = m_messageData->command;
  SCHAT_DEBUG_STREAM(this << "command()" << command << m_packetsEvent->userId().toHex())

  if (command.isEmpty())
    return false;

  if (command == QLatin1String("join")) {
    readJoinCmd();
    return true;
  }

  if (command == QLatin1String("part")) {
    if (m_storage->removeUserFromChannel(m_reader->sender(), m_reader->dest()))
      return false;

    return true;
  }

  if (command == QLatin1String("status")) {
    return updateUserStatus();
  }

  return false;
}


/*!
 * Обработка команды "join".
 */
bool Core::readJoinCmd()
{
  ChatChannel chan;
  if (!m_messageData->destId().isEmpty())
    chan = m_storage->channel(m_messageData->destId());

  if (!chan)
    chan = channel(m_messageData->text);

  if (!chan)
    return false;

  return join(m_reader->sender(), chan);
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

  if (route()) {
    acceptMessage();
    return true;
  }
  else {
    rejectMessage(NoticeData::UnknownError);
    return false;
  }
}


void Core::acceptMessage()
{
  if (m_messageData->name == 0)
    return;

  if (SimpleID::typeOf(m_reader->dest()) == SimpleID::UserId && m_storage->isSameSlave(m_reader->dest(), m_reader->sender()))
    return;

  NoticeData data(m_reader->dest(), m_reader->sender(), NoticeData::MessageDelivered, m_messageData->name);
  send(m_storage->user(m_reader->sender()), NoticeWriter(m_sendStream, data).data());
}


void Core::rejectMessage(int reason)
{
  SCHAT_DEBUG_STREAM("rejectMessage()" << reason)

  if (m_messageData->name == 0)
    return;

  NoticeData data(m_reader->dest(), m_reader->sender(), NoticeData::MessageRejected, m_messageData->name, reason);
  send(m_storage->user(m_reader->sender()), NoticeWriter(m_sendStream, data).data());
}
