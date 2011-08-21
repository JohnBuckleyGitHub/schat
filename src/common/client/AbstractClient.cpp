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

#include <QBasicTimer>
#include <QTimerEvent>

#include "Channel.h"
#include "client/AbstractClient.h"
#include "client/AbstractClient_p.h"
#include "debugstream.h"
#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "net/packets/channels.h"
#include "net/packets/message.h"
#include "net/packets/notices.h"
#include "net/packets/users.h"
#include "net/PacketWriter.h"
#include "net/Protocol.h"
#include "User.h"


AbstractClientPrivate::AbstractClientPrivate()
  : sendLock(false)
  , user(new User())
  , reconnects(0)
  , reconnectTimer(new QBasicTimer())
  , uniqueId(SimpleID::uniqueId())
  , serverData(new ServerData())
  , clientState(AbstractClient::ClientOffline)
  , previousState(AbstractClient::ClientOffline)
{
  user->setUserAgent(SimpleID::userAgent());
}


AbstractClientPrivate::~AbstractClientPrivate()
{
  if (reconnectTimer->isActive())
    reconnectTimer->stop();

  delete reconnectTimer;
  delete serverData;
}


QString AbstractClientPrivate::mangleNick()
{
  int rand = qrand() % 89 + 10;
  if (nick.isEmpty())
    nick = user->nick();

  return nick.left(User::MaxNickLength - 2) + QString::number(rand);
}


/*!
 * Чтение пакета AuthReplyPacket.
 */
bool AbstractClientPrivate::readAuthReply()
{
  AuthReplyData data = AuthReplyReader(reader).data;

  SCHAT_DEBUG_STREAM(this << "AuthReply" << data.status << data.error << data.userId.toHex())

  Q_Q(AbstractClient);

  if (data.status == AuthReplyData::AccessGranted) {
    q->setAuthorized(data.userId);
    user->setId(data.userId);
    user->setHost(data.host);

    setServerData(data.serverData);

    if (user->status() == User::OfflineStatus)
      user->setStatus(User::OnlineStatus);

    ClientChannel channel = ClientChannel(new Channel(SimpleID::setType(SimpleID::ChannelId, userId), QLatin1String("~") + user->nick()));
    addChannel(channel);

    emit(q->userDataChanged(user->id()));
    emit(q->ready());
    return true;
  }
  else if (data.status == AuthReplyData::AccessDenied) {
    if (data.error == AuthReplyData::NickAlreadyUse) {
      user->setNick(mangleNick());
      q->requestAuth();
    }
  }

  return false;
}


/*!
 * Установка состояния клиента.
 */
void AbstractClientPrivate::setClientState(AbstractClient::ClientState state)
{
  if (clientState == state)
    return;

  previousState = clientState;
  clientState = state;

  if (state == AbstractClient::ClientOnline || state == AbstractClient::ClientOffline)
    reconnects = 0;

  if (state == AbstractClient::ClientOnline) {
    users.insert(user->id(), user);
  }
  else {
    foreach (ClientChannel chan, channels) {
      chan->clear();
    }
  }

  Q_Q(AbstractClient);
  emit(q->clientStateChanged(state, previousState));
}


/*!
 * Установка идентификатора сервера, при успешной авторизации.
 *
 * В случае повторного подключения к предыдущему серверу происходит,
 * вход в раннее открытые каналы.
 * В случае если подключение происходит к новому серверу, таблица каналов очищается.
 *
 * \param data Данные сервера.
 */
void AbstractClientPrivate::setServerData(const ServerData &data)
{
  bool sameServer = false;

  if (!serverData->id().isEmpty() && serverData->id() == data.id())
    sameServer = true;

  serverData->setId(data.id());
  serverData->setName(data.name());
  serverData->setChannelId(data.channelId());
  serverData->setFeatures(data.features());

  setClientState(AbstractClient::ClientOnline);

  if (!sameServer)
    setup();
  else
    restore();
}


/*!
 * Добавление нового канала.
 * В случае если идентификатор канала уже используется, возможны 2 сценария:
 * - Если существующий канал содержит пользователей, добавления не произойдёт,
 * объект \p channel будет удалён и функция возвратит false.
 * - Существующий канал не содержит пользователей, что означает его не валидность,
 * объект канала будет удалён и его место в таблице каналов займёт \p channel.
 *
 * \return true в случае успешного добавления канала.
 */
bool AbstractClientPrivate::addChannel(ClientChannel channel)
{
  QByteArray id = channel->id();
  ClientChannel ch = channels.value(id);

  if (ch && ch->userCount())
    return false;

  channels[id] = channel;
  channel->addUser(user->id());
  user->addChannel(id);

  if (channel->userCount() == 1) {
    endSyncChannel(channel);
    return true;
  }

  QList<QByteArray> list = channel->users();
  list.removeAll(userId);
  int unsync = 0;

  for (int i = 0; i < list.size(); ++i) {
    ClientUser u = users.value(list.at(i));
    if (u) {
      u->addChannel(id);
    }
    else
      unsync++;
  }

  if (unsync == 0)
    endSyncChannel(channel);

  return true;
}


/*!
 * Обработка подключения к каналу, в случае успеха канал добавляется в таблицу каналов.
 */
bool AbstractClientPrivate::readChannel()
{
  ChannelReader reader(this->reader);

  SCHAT_DEBUG_STREAM(this << "readChannel()" << reader.channel->id().toHex())

  if (!reader.channel->isValid())
    return false;

  addChannel(ClientChannel(reader.channel));
  return true;
}


/*!
 * Завершение синхронизации канала.
 */
void AbstractClientPrivate::endSyncChannel(ClientChannel channel)
{
  if (!channel)
    return;

  if (channel->isSynced())
    return;

  channel->setSynced(true);

  Q_Q(AbstractClient);
  emit(q->synced(channel->id()));
}


/*!
 * Завершение синхронизации канала.
 */
void AbstractClientPrivate::endSyncChannel(const QByteArray &id)
{
  endSyncChannel(channels.value(id));
}


/*!
 * Обработка команд.
 *
 * \return true в случае если команда была обработана, иначе false.
 */
bool AbstractClientPrivate::command()
{
  QString command = messageData->command;
  SCHAT_DEBUG_STREAM(this << "command()" << command)

  if (command == QLatin1String("part")) {
    removeUserFromChannel(reader->dest(), reader->sender());
    return true;
  }

  if (command == QLatin1String("synced")) {
    endSyncChannel(reader->sender());
    return true;
  }

  if (command == QLatin1String("leave")) {
    removeUser(reader->sender());
    return true;
  }

  if (command == QLatin1String("status")) {
    updateUserStatus(messageData->text);
    return true;
  }

  return false;
}


/*!
 * Чтение сообщения и обработка поддерживаемых команд.
 * В случае если сообщении не содержало команд или команда
 * не была обработана, высылается сигнал о новом сообщении.
 */
bool AbstractClientPrivate::readMessage()
{
  SCHAT_DEBUG_STREAM(this << "readMessage()")

  MessageReader reader(this->reader);
  messageData = &reader.data;
  messageData->timestamp = timestamp;

  if (messageData->options & MessageData::ControlOption && command()) {
    return true;
  }

  SCHAT_DEBUG_STREAM("      " << reader.data.text)

  Q_Q(AbstractClient);
  emit(q->message(reader.data));

  return true;
}


bool AbstractClientPrivate::readNotice()
{
  SCHAT_DEBUG_STREAM(this << "readNotice()")

  Q_Q(AbstractClient);
  NoticeReader reader(this->reader);
  reader.data.timestamp = timestamp;
  emit(q->notice(reader.data));

  return true;
}


/*!
 * Чтение пакета Packet::UserData.
 *
 * В случае если адрес назначения канал, то требуется чтобы текущий пользователь был подключен к нему заранее,
 * также будет произведена попытка добавления пользователя в него и в случае успеха будет выслан сигнал.
 * Идентификатор нового пользователя не может быть пустым и в случае если новый пользователь отсутствует
 * в таблице пользователей, то будет произведена попытка его добавить.
 */
bool AbstractClientPrivate::readUserData()
{
  SCHAT_DEBUG_STREAM(this << "readUserData()")

  /// Идентификатор отправителя не может быть пустым.
  QByteArray id = reader->sender();
  if (id.isEmpty())
    return false;

  UserReader reader(this->reader);
  if (!reader.user.isValid())
    return false;

  /// Если пользователь не существует, то он будет создан, иначе произойдёт обновление данных,
  /// В обоих случаях будет выслан сигнал userDataChanged().
  Q_Q(AbstractClient);
  ClientUser user = users.value(id);
  if (!user) {
    user = ClientUser(new User(&reader.user));
    users.insert(id, user);

    emit(q->userDataChanged(id));
  }
  else {
    updateUserData(user, &reader.user);
  }

  QByteArray channelId = this->reader->channel();
  if (channelId.isEmpty() && SimpleID::typeOf(this->reader->dest()) == SimpleID::ChannelId)
      channelId = this->reader->dest();

  if (!channelId.isEmpty()) {
    ClientChannel channel = channels.value(channelId);
    if (!channel)
      return false;

    user->addChannel(channelId);
    if (channel->addUser(id) || channel->isSynced())
      emit(q->join(channelId, id));
  }

  return true;
}


/*!
 * Удаление пользователя.
 */
bool AbstractClientPrivate::removeUser(const QByteArray &userId)
{
  ClientUser user = users.value(userId);
  if (!user)
    return false;

  user->setStatus(User::OfflineStatus);

  Q_Q(AbstractClient);
  emit(q->userLeave(userId));

  QList<QByteArray> channels = user->channels();
  for (int i = 0; i < channels.size(); ++i) {
    removeUserFromChannel(channels.at(i), userId, false);
  }

  users.remove(userId);
  return true;
}


/*!
 * Удаление пользователя из канала.
 */
bool AbstractClientPrivate::removeUserFromChannel(const QByteArray &channelId, const QByteArray &userId, bool clear)
{
  Q_Q(AbstractClient);
  ClientUser user = users.value(userId);
  ClientChannel channel = channels.value(channelId);

  if (!user.isNull() && channel) {
    channel->removeUser(user->id());
    user->removeChannel(channel->id());

    emit(q->part(channel->id(), user->id()));
    if (clear && user->channelsCount() == 1)
      removeUser(userId);

    return true;
  }

  return false;
}


/*!
 * Обновление информации о пользователе.
 */
void AbstractClientPrivate::updateUserData(ClientUser existUser, User *user)
{
  SCHAT_DEBUG_STREAM(this << "updateUserData()");

  Q_Q(AbstractClient);
  if (existUser == this->user && this->user->nick() != user->nick()) {
    q->setNick(user->nick());
  }

  existUser->setNick(user->nick());
  existUser->setRawGender(user->rawGender());
  existUser->setStatus(user->status());

  emit(q->userDataChanged(existUser->id()));
}


/*!
 * Обновление статуса пользователя.
 */
void AbstractClientPrivate::updateUserStatus(const QString &text)
{
  Q_Q(AbstractClient);

  ClientUser user = q->user(reader->sender());
  if (!user)
    return;

  if (!user->setStatus(text))
    return;

  if (user->status() == User::OfflineStatus) {
    user->setStatus(User::OnlineStatus);
  }

  emit(q->userDataChanged(user->id()));
}


AbstractClient::AbstractClient(QObject *parent)
  : SimpleSocket(*new AbstractClientPrivate(), parent)
{
  connect(this, SIGNAL(requestAuth(quint64)), SLOT(requestAuth()));
  connect(this, SIGNAL(released(quint64)), SLOT(released()));
}


AbstractClient::AbstractClient(AbstractClientPrivate &dd, QObject *parent)
  : SimpleSocket(dd, parent)
{
  connect(this, SIGNAL(requestAuth(quint64)), SLOT(requestAuth()));
  connect(this, SIGNAL(released(quint64)), SLOT(released()));
}


AbstractClient::~AbstractClient()
{
}


/*!
 * Установка подключения к серверу.
 */
bool AbstractClient::openUrl(const QUrl &url)
{
  SCHAT_DEBUG_STREAM(this << "openUrl()" << url.toString())

  Q_D(AbstractClient);
  d->url = url;

  if (!d->url.isValid())
    return false;

  if (d->url.scheme() != "schat")
    return false;

  if (d->reconnectTimer->isActive())
    d->reconnectTimer->stop();

  if (state() != QAbstractSocket::UnconnectedState) {
    leave();
  }

  if (!d->nick.isEmpty())
    d->user->setNick(d->nick);

  d->setClientState(ClientConnecting);
  connectToHost(url.host(), url.port(Protocol::DefaultPort));
  return true;
}


/*!
 * Отправка сообщения.
 */
bool AbstractClient::send(const MessageData &data, bool echo)
{
  Q_D(AbstractClient);
  return send(MessageWriter(d->sendStream, data, echo).data());
}


bool AbstractClient::send(const QByteArray &packet)
{
  Q_D(AbstractClient);
  if (d->sendLock) {
    d->sendQueue.append(packet);
    return true;
  }

  return SimpleSocket::send(packet);
}



bool AbstractClient::send(const QList<QByteArray> &packets)
{
  Q_D(AbstractClient);
  if (d->sendLock) {
    d->sendQueue.append(packets);
    return true;
  }

  return SimpleSocket::send(packets);
}


ClientChannel AbstractClient::channel(const QByteArray &id) const
{
  Q_D(const AbstractClient);
  return d->channels.value(id);
}


AbstractClient::ClientState AbstractClient::clientState() const
{
  Q_D(const AbstractClient);
  return d->clientState;
}


ClientUser AbstractClient::user() const
{
  Q_D(const AbstractClient);
  return d->user;
}


ClientUser AbstractClient::user(const QByteArray &id) const
{
  Q_D(const AbstractClient);
  return d->users.value(id);
}


PacketReader *AbstractClient::reader()
{
  Q_D(const AbstractClient);
  return d->reader;
}


QByteArray AbstractClient::serverId() const
{
  Q_D(const AbstractClient);
  return d->serverData->id();
}


QByteArray AbstractClient::uniqueId() const
{
  Q_D(const AbstractClient);
  return d->uniqueId;
}


/*!
 * Получение оригинального ника, не искажённого функцией автоматического
 * разрешения коллизий.
 */
QString AbstractClient::nick() const
{
  Q_D(const AbstractClient);
  if (d->nick.isEmpty())
    return d->user->nick();

  return d->nick;
}


QUrl AbstractClient::url() const
{
  Q_D(const AbstractClient);
  return d->url;
}


ServerData *AbstractClient::serverData()
{
  Q_D(const AbstractClient);
  return d->serverData;
}


void AbstractClient::lock()
{
  Q_D(AbstractClient);
  d->sendLock = true;
}


void AbstractClient::setNick(const QString &nick)
{
  Q_D(AbstractClient);
  d->user->setNick(nick);
  d->nick = "";
}


void AbstractClient::unlock()
{
  Q_D(AbstractClient);
  if (!d->sendQueue.isEmpty()) {
    SimpleSocket::send(d->sendQueue);
    d->sendQueue.clear();
  }

  d->sendLock = false;
}


void AbstractClient::leave()
{
  SCHAT_DEBUG_STREAM(this << "leave()")

  Q_D(AbstractClient);

  if (d->reconnectTimer->isActive())
    d->reconnectTimer->stop();

  d->setClientState(ClientOffline);
  SimpleSocket::leave();
}


/*!
 * Отключение от канала.
 */
void AbstractClient::part(const QByteArray &channelId)
{
  SCHAT_DEBUG_STREAM(this << "part()" << channelId.toHex())

  Q_D(AbstractClient);

  if (!d->channels.contains(channelId))
    return;

  d->channels.remove(channelId);
  d->user->removeChannel(channelId);

  MessageData message(userId(), channelId, QLatin1String("part"), QString());
  send(message);
}


/*!
 * Обработка пакетов.
 */
void AbstractClient::newPacketsImpl()
{
  Q_D(AbstractClient);
  SCHAT_DEBUG_STREAM(this << "newPacketsImpl()" << d->readQueue.size())

  while (!d->readQueue.isEmpty()) {
    d->readBuffer = d->readQueue.takeFirst();
    PacketReader reader(d->readStream);
    d->reader = &reader;

    if (isAuthorized())
      emit packetReady(reader.type());
    else if (reader.type() == Protocol::AuthReplyPacket)
      d->readAuthReply();
  }
}


void AbstractClient::timerEvent(QTimerEvent *event)
{
  Q_D(AbstractClient);
  if (event->timerId() == d->reconnectTimer->timerId()) {
    openUrl(d->url);
    return;
  }

  SimpleSocket::timerEvent(event);
}


/*!
 * Обработка разрыва соединения.
 */
void AbstractClient::released()
{
  SCHAT_DEBUG_STREAM(this << "released()" << errorString() << isAuthorized())

  Q_D(AbstractClient);
  if (d->reconnectTimer->isActive())
    d->reconnectTimer->stop();

  if (d->clientState == ClientOffline)
    return;

  if (d->clientState == ClientOnline) {
    d->setClientState(ClientOffline);
    d->setClientState(ClientConnecting);
  }

  if (d->reconnects < Protocol::MaxFastReconnects) {
    d->reconnectTimer->start(Protocol::FastReconnectTime, this);
  }
  else if (d->reconnects < Protocol::MaxFastReconnects + Protocol::MaxNormalReconnects) {
    d->reconnectTimer->start(Protocol::NormalReconnectTime, this);
  }
  else {
    d->reconnectTimer->start(Protocol::SlowReconnectTime, this);
  }

  ++d->reconnects;
}


void AbstractClient::requestAuth()
{
  emit requestClientAuth();
}
