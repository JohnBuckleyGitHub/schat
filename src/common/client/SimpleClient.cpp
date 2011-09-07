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

#include "debugstream.h"

#include "client/SimpleClient.h"
#include "client/SimpleClient_p.h"
#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "net/packets/channels.h"
#include "net/packets/message.h"
#include "net/packets/notices.h"
#include "net/packets/users.h"

SimpleClientPrivate::SimpleClientPrivate()
{
}


SimpleClientPrivate::~SimpleClientPrivate()
{
}


/*!
 * Очистка данных состояния клиента.
 */
void SimpleClientPrivate::clearClient()
{
  user->clear();
  users.clear();
  users.insert(user->id(), user);

  channels.clear();
}


bool SimpleClientPrivate::readAuthReply()
{
  if (AbstractClientPrivate::readAuthReply()) {
    Q_Q(SimpleClient);
    ClientChannel channel = ClientChannel(new Channel(SimpleID::setType(SimpleID::ChannelId, userId), QLatin1String("~") + user->nick()));
    addChannel(channel);

    emit(q->userDataChanged(user->id()));
    return true;
  }

  return false;
}


/*!
 * Восстановление состояния клиента после повторного подключения к предыдущему серверу.
 */
void SimpleClientPrivate::restore()
{
  Q_Q(SimpleClient);
  q->lock();

  /// Клиент заново входит в ранее открытие каналы.
  if (!channels.isEmpty()) {
    MessageData data(userId, QByteArray(), QLatin1String("join"), QString());
    data.options |= MessageData::TextOption;

    QHashIterator<QByteArray, ClientChannel> i(channels);
    while (i.hasNext()) {
      i.next();
      data.dest = QList<QByteArray>() << i.key();
      data.text = i.value()->name();
      q->send(MessageWriter(sendStream, data).data());
    }
  }

  clearClient();

  q->send(MessageWriter(sendStream, MessageData(userId, QByteArray(), QLatin1String("ready"), QLatin1String("restore"))).data());
  q->unlock();
}


void SimpleClientPrivate::setClientState(AbstractClient::ClientState state)
{
  if (clientState == state)
    return;

  if (state == AbstractClient::ClientOnline) {
    users.insert(user->id(), user);
  }
  else {
    foreach (ClientChannel chan, channels) {
      chan->clear();
    }
  }

  AbstractClientPrivate::setClientState(state);
}


void SimpleClientPrivate::setup()
{
  clearClient();

  Q_Q(SimpleClient);
  q->lock();

  if (serverData->features() & ServerData::AutoJoinSupport && !serverData->channelId().isEmpty()) {
    MessageData data(user->id(), serverData->channelId(), QLatin1String("join"), QString());
    q->send(MessageWriter(sendStream, data).data());
  }

  q->send(MessageWriter(sendStream, MessageData(userId, QByteArray(), QLatin1String("ready"), QLatin1String("setup"))).data());
  q->unlock();
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
bool SimpleClientPrivate::addChannel(ClientChannel channel)
{
  QByteArray id = channel->id();
  ClientChannel ch = channels.value(id);

  if (ch && ch->userCount())
    return false;

  channels[id] = channel;
  channel->addUser(user->id());
  user->addChannel(id);

  Q_Q(SimpleClient);

  if (channel->userCount() == 1) {
    if (!channel->name().startsWith(QLatin1String("~")))
      emit(q->join(id));

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

  if (!channel->name().startsWith(QLatin1String("~")))
    emit(q->join(id));

  if (unsync == 0)
    endSyncChannel(channel);

  return true;
}


/*!
 * Обработка подключения к каналу, в случае успеха канал добавляется в таблицу каналов.
 */
bool SimpleClientPrivate::readChannel()
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
void SimpleClientPrivate::endSyncChannel(ClientChannel channel)
{
  if (!channel)
    return;

  if (channel->isSynced())
    return;

  channel->setSynced(true);

  if (channel->name().startsWith(QLatin1String("~")))
    return;

  Q_Q(SimpleClient);
  emit(q->synced(channel->id()));
}


/*!
 * Завершение синхронизации канала.
 */
void SimpleClientPrivate::endSyncChannel(const QByteArray &id)
{
  endSyncChannel(channels.value(id));
}


/*!
 * Обработка команд.
 *
 * \return true в случае если команда была обработана, иначе false.
 */
bool SimpleClientPrivate::command()
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

  if (command == QLatin1String("split") && SimpleID::typeOf(reader->sender()) == SimpleID::ServerId) {
    split();
    return true;
  }

  return false;
}


/*!
 * Чтение сообщения и обработка поддерживаемых команд.
 * В случае если сообщении не содержало команд или команда
 * не была обработана, высылается сигнал о новом сообщении.
 */
bool SimpleClientPrivate::readMessage()
{
  SCHAT_DEBUG_STREAM(this << "readMessage()")

  MessageReader reader(this->reader);
  messageData = &reader.data;

  if (!(messageData->flags & MessageData::OfflineFlag))
    messageData->timestamp = timestamp;

  if (messageData->options & MessageData::ControlOption && command()) {
    return true;
  }

  SCHAT_DEBUG_STREAM("      " << reader.data.text)

  Q_Q(SimpleClient);
  emit(q->message(reader.data));

  return true;
}


void SimpleClientPrivate::split()
{
  QList<QByteArray> out;
  int number = serverData->number();
  QHashIterator<QByteArray, ClientUser> i(users);

  if (!messageData->text.isEmpty()) {
    number = messageData->text.toInt();
    if (number == 0)
      return;

    while (i.hasNext()) {
      i.next();
      if (i.value()->serverNumber() == number)
        out.append(i.key());
    }
  }
  else {
    while (i.hasNext()) {
      i.next();
      if (i.value()->serverNumber() != number)
        out.append(i.key());
    }
  }

  if (out.isEmpty())
    return;

  Q_Q(SimpleClient);
  emit(q->split(out));

  for (int i = 0; i < out.size(); ++i) {
    users.remove(out.at(i));
  }
}


bool SimpleClientPrivate::readNotice()
{
  Q_Q(SimpleClient);

  quint16 type = reader->get<quint16>();
  if (type == AbstractNotice::MessageNoticeType) {
    MessageNotice notice(type, reader);
    emit(q->notice(notice));
  }
  else
    emit(q->notice(type));

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
bool SimpleClientPrivate::readUserData()
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
  Q_Q(SimpleClient);
  ClientUser user = users.value(id);
  if (!user) {
    user = ClientUser(new User(&reader.user));
    users.insert(id, user);

    emit(q->userDataChanged(id));
  }
  else {
    updateUserData(user, reader);
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
bool SimpleClientPrivate::removeUser(const QByteArray &userId)
{
  ClientUser user = users.value(userId);
  if (!user)
    return false;

  user->setStatus(User::OfflineStatus);

  Q_Q(SimpleClient);
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
bool SimpleClientPrivate::removeUserFromChannel(const QByteArray &channelId, const QByteArray &userId, bool clear)
{
  Q_Q(SimpleClient);
  ClientUser user = users.value(userId);
  ClientChannel channel = channels.value(channelId);

  if (!user.isNull() && channel) {
    channel->removeUser(user->id());
    user->removeChannel(channel->id());

    if (clear) {
      emit(q->part(channel->id(), user->id()));
      if (user->channelsCount() == 1)
        removeUser(userId);
    }

    return true;
  }

  return false;
}


/*!
 * Обновление информации о пользователе.
 */
void SimpleClientPrivate::updateUserData(ClientUser existUser, UserReader &reader)
{
  SCHAT_DEBUG_STREAM(this << "updateUserData()");

  User *user = &reader.user;
  Q_Q(SimpleClient);
  if (existUser == this->user && this->user->nick() != user->nick()) {
    q->setNick(user->nick());
  }

  existUser->setNick(user->nick());
  existUser->setRawGender(user->rawGender());
  existUser->setStatus(user->status());

  if (reader.options && UserWriter::StaticData) {
    existUser->setUserAgent(user->userAgent());
    existUser->setHost(user->host());
    existUser->setServerNumber(user->serverNumber());
  }

  emit(q->userDataChanged(existUser->id()));
}


/*!
 * Обновление статуса пользователя.
 */
void SimpleClientPrivate::updateUserStatus(const QString &text)
{
  Q_Q(SimpleClient);

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



SimpleClient::SimpleClient(QObject *parent)
  : AbstractClient(*new SimpleClientPrivate(), parent)
{
}


SimpleClient::SimpleClient(SimpleClientPrivate &dd, QObject *parent)
  : AbstractClient(dd, parent)
{
}


SimpleClient::~SimpleClient()
{
}


ClientChannel SimpleClient::channel(const QByteArray &id) const
{
  Q_D(const SimpleClient);
  return d->channels.value(id);
}

ClientUser SimpleClient::user() const
{
  return AbstractClient::user();
}


ClientUser SimpleClient::user(const QByteArray &id) const
{
  Q_D(const SimpleClient);
  return d->users.value(id);
}


void SimpleClient::leave()
{
  MessageData data(userId(), AbstractClient::user()->channels(), QLatin1String("leave"), QString());
  send(data);

  AbstractClient::leave();
}


/*!
 * Отключение от канала.
 */
void SimpleClient::part(const QByteArray &channelId)
{
  SCHAT_DEBUG_STREAM(this << "part()" << channelId.toHex())

  Q_D(SimpleClient);

  if (!d->channels.contains(channelId))
    return;

  d->channels.remove(channelId);
  d->user->removeChannel(channelId);

  MessageData message(userId(), channelId, QLatin1String("part"), QString());
  send(message);
}


void SimpleClient::requestAuth()
{
  SCHAT_DEBUG_STREAM(this << "requestAuth()")

  Q_D(SimpleClient);

  AuthRequestData data(AuthRequestData::Anonymous, d->url.host(), d->user.data());
  data.uniqueId = d->uniqueId;
  send(AuthRequestWriter(d->sendStream, data).data());
}


/*!
 * Обработка пакетов.
 */
void SimpleClient::newPacketsImpl()
{
  Q_D(SimpleClient);
  SCHAT_DEBUG_STREAM(this << "newPacketsImpl()" << d->readQueue.size())

  while (!d->readQueue.isEmpty()) {
    d->readBuffer = d->readQueue.takeFirst();
    PacketReader reader(d->readStream);
    d->reader = &reader;

    switch (reader.type()) {
      case Protocol::AuthReplyPacket:
        d->readAuthReply();
        break;

      case Protocol::ChannelPacket:
        d->readChannel();
        break;

      case Protocol::MessagePacket:
        d->readMessage();
        break;

      case Protocol::UserDataPacket:
        d->readUserData();
        break;

      case Protocol::NoticePacket:
        d->readNotice();
        break;

      default:
        break;
    }
  }
}
