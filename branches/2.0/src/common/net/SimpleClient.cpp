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
#include "debugstream.h"
#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "net/packets/channels.h"
#include "net/packets/message.h"
#include "net/packets/users.h"
#include "net/PacketWriter.h"
#include "net/SimpleClient.h"
#include "Protocol.h"
#include "User.h"

SimpleClient::SimpleClient(User *user, quint64 id, QObject *parent)
  : SimpleSocket(id, parent)
  , m_syncChannelMode(false)
  , m_sendLock(false)
  , m_clientState(ClientOffline)
  , m_reconnects(0)
  , m_user(user)
{
  m_reconnectTimer = new QBasicTimer;

  connect(this, SIGNAL(requestAuth(quint64)), SLOT(requestAuth()));
  connect(this, SIGNAL(released(quint64)), SLOT(released()));
}


SimpleClient::~SimpleClient()
{
  if (m_reconnectTimer->isActive())
    m_reconnectTimer->stop();

  delete m_reconnectTimer;
  delete m_user;
}


/*!
 * Установка подключения к серверу.
 */
bool SimpleClient::openUrl(const QUrl &url)
{
  SCHAT_DEBUG_STREAM(this << "openUrl()" << url.toString())

  m_url = url;

  if (m_reconnectTimer->isActive())
    m_reconnectTimer->stop();

  if (state() != QAbstractSocket::UnconnectedState)
    return false;

  if (!m_url.isValid())
    return false;

  if (m_url.scheme() != "schat")
    return false;

  setClientState(ClientConnecting);
  connectToHost(url.host(), url.port(Protocol::DefaultPort));

  return true;
}


bool SimpleClient::send(const QByteArray &packet)
{
  if (m_sendLock) {
    m_sendQueue.append(packet);
    return true;
  }

  return SimpleSocket::send(packet);
}



bool SimpleClient::send(const QList<QByteArray> &packets)
{
  if (m_sendLock) {
    m_sendQueue.append(packets);
    return true;
  }

  return SimpleSocket::send(packets);
}


/*!
 * Отправка сообщения.
 */
bool SimpleClient::sendMessage(int destType, const QByteArray &dest, const QString &message)
{
  SCHAT_DEBUG_STREAM(this << "sendMessage()" << destType << dest << message)

  Message msg("join", message);
  send(msg.data(m_sendStream));
  return true;
}


void SimpleClient::leave()
{
  SCHAT_DEBUG_STREAM(this << "leave()")

  if (m_reconnectTimer->isActive())
    m_reconnectTimer->stop();

  setClientState(ClientOffline);
  SimpleSocket::leave();
  setAuthorized(QByteArray());
}


/*!
 * Отключение от канала.
 */
void SimpleClient::part(const QByteArray &channelId)
{
  SCHAT_DEBUG_STREAM(this << "part()" << channelId.toHex())

  if (!m_channels.contains(channelId))
    return;

  m_channels.remove(channelId);
  user()->removeChannel(channelId);

  PacketWriter writer(m_sendStream, Protocol::MessagePacket, userId(), channelId);
  writer.put<quint8>(Packet::ControlMessage);
  writer.put("part");
  send(writer.data());
}


/*!
 * Обработка пакетов.
 */
void SimpleClient::newPacketsImpl()
{
  SCHAT_DEBUG_STREAM(this << "newPacketsImpl()" << m_readQueue.size())

  while (!m_readQueue.isEmpty()) {
    m_readBuffer = m_readQueue.takeFirst();
    PacketReader reader(m_readStream);
    m_reader = &reader;

    switch (reader.type()) {
      case Protocol::AuthReplyPacket:
        readAuthReply();
        break;

      case Protocol::JoinReplyPacket:
        readJoinReply();
        break;

      case Protocol::MessagePacket:
        readMessage();
        break;

      case Protocol::UserDataPacket:
        readUserData();
        break;

      default:
        break;
    }
  }

  if (m_syncChannelMode && !m_syncChannelCache.isEmpty()) {
    emit join(m_syncChannelId, m_syncChannelCache);
    m_syncChannelCache.clear();
  }
}


void SimpleClient::timerEvent(QTimerEvent *event)
{
  SCHAT_DEBUG_STREAM(this << "timerEvent()" << m_reconnects)

  if (event->timerId() == m_reconnectTimer->timerId()) {
    openUrl(m_url);
    return;
  }

  SimpleSocket::timerEvent(event);
}


/*!
 * Обработка разрыва соединения.
 */
void SimpleClient::released()
{
  SCHAT_DEBUG_STREAM(this << "released()" << errorString() << isAuthorized())

  if (m_reconnectTimer->isActive())
    m_reconnectTimer->stop();

  if (m_clientState == ClientOffline)
    return;

  if (m_clientState == ClientOnline)
    setClientState(ClientConnecting);

  if (m_reconnects < Protocol::MaxFastReconnects) {
    m_reconnectTimer->start(Protocol::FastReconnectTime, this);
  }
  else if (m_reconnects < Protocol::MaxFastReconnects + Protocol::MaxNormalReconnects) {
    m_reconnectTimer->start(Protocol::NormalReconnectTime, this);
  }
  else {
    m_reconnectTimer->start(Protocol::SlowReconnectTime, this);
  }

  ++m_reconnects;
}


void SimpleClient::requestAuth()
{
  SCHAT_DEBUG_STREAM(this << "requestAuth()")

  AuthRequest request(AuthRequest::Anonymous, m_url.host(), m_user->nick());
  send(request.data(m_sendStream));
}


bool SimpleClient::removeUser(const QByteArray &userId)
{
  User *user = m_users.value(userId);
  if (!user)
    return false;

  QList<QByteArray> channels = user->channels();
  for (int i = 0; i < channels.size(); ++i) {
    removeUserFromChannel(channels.at(i), userId);
  }

  delete user;
  m_users.remove(userId);
  return true;
}


/*!
 * Удаление пользователя из канала.
 */
bool SimpleClient::removeUserFromChannel(const QByteArray &channelId, const QByteArray &userId)
{
  User *user = m_users.value(userId);
  Channel *channel = m_channels.value(channelId);

  if (user && channel) {
    channel->removeUser(user->id());
    user->removeChannel(channel->id());
    emit part(channel->id(), user->id());
    return true;
  }

  return false;
}


/*!
 * Удаление всех пользователей, при отключении от сервера.
 */
void SimpleClient::removeAllUsers()
{
  m_users.remove(userId());
  qDeleteAll(m_users);
  m_users.clear();

  foreach (Channel *chan, m_channels) {
    chan->clear();
  }
}


/*!
 * Установка состояния клиента.
 */
void SimpleClient::setClientState(ClientState state)
{
  if (m_clientState == state)
    return;

  m_clientState = state;

  if (state == ClientOnline || state == ClientOffline)
    m_reconnects = 0;

  if (state == ClientOnline) {
    m_users.insert(userId(), m_user);
  }
  else {
    removeAllUsers();
  }

  emit clientStateChanged(m_clientState);
}


void SimpleClient::unlock()
{
  if (!m_sendQueue.isEmpty()) {
    SimpleSocket::send(m_sendQueue);
    m_sendQueue.clear();
  }

  m_sendLock = false;
}


/*!
 * Обработка пакета Protocol::Message с опцией Packet::ControlMessage.
 */
bool SimpleClient::command()
{
  QString command = m_reader->text();
  SCHAT_DEBUG_STREAM(this << "command()" << command)

  if (command == "part") {
    return removeUserFromChannel(m_reader->dest(), m_reader->sender());
  }

  if (command == "BSCh") {
    lock();
    m_syncChannelId = m_reader->dest();
    m_syncChannelMode = true;
    return true;
  }

  if (command == "ESCh") {
    unlock();
    m_syncChannelCache.append(QByteArray());

    if (!m_syncChannelCache.isEmpty()) {
      emit join(m_syncChannelId, m_syncChannelCache);
      m_syncChannelCache.clear();
    }

    m_syncChannelMode = false;
    return true;
  }

  if (command == "leave") {
    return removeUser(m_reader->sender());
  }

  return true;
}


/*!
 * Чтение пакета Packet::AuthReply.
 */
bool SimpleClient::readAuthReply()
{
  AuthReply authReply(m_reader);

  SCHAT_DEBUG_STREAM(this << "AuthReply" << authReply.status() << authReply.error() << authReply.userId().toHex())

  if (authReply.status() == AuthReply::AccessGranted) {
    m_serverId = authReply.serverId();
    setAuthorized(authReply.userId());
    setClientState(ClientOnline);
    return true;
  }

  return false;
}


/*!
 * Чтение пакета Packet::JoinReply.
 *
 * Обработка подключения к каналу, в случае успеха канал добавляется в таблицу каналов.
 */
bool SimpleClient::readJoinReply()
{
  JoinReply joinReply(m_reader);
  if (!joinReply.isValid())
    return false;

  Channel *chan = joinReply.channel();
  if (m_channels.contains(chan->id())) {
    delete chan;
    return false;
  }

  m_channels.insert(chan->id(), chan);
  chan->addUser(userId());
  user()->addChannel(chan->id());
  emit join(chan->id(), QByteArray());
  return true;
}


/*!
 * Чтение пакета Packet::Message.
 */
bool SimpleClient::readMessage()
{
  int options = m_reader->get<quint8>();
  if (options == Packet::ControlMessage)
    command();

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
bool SimpleClient::readUserData()
{
  SCHAT_DEBUG_STREAM(this << "readUserData()")

  QByteArray dest = m_reader->dest();
  int idType = Packet::idType(dest);

  if (idType == Protocol::ChannelId && !m_channels.contains(dest))
    return false;

  QByteArray id = m_reader->sender();
  if (id.isEmpty())
    return false;

  User *user = m_users.value(id);
  if (!user) {
    UserData userData(m_reader);
    if (!userData.isValid())
      return false;

    user = userData.user();
    m_users.insert(id, user);
  }

  if (idType == Protocol::ChannelId) {
    Channel *chan = m_channels.value(dest);
    if (!chan)
      return false;

    user->addChannel(dest);
    chan->addUser(id);

    if (m_syncChannelMode)
      m_syncChannelCache.append(id);
    else
      emit join(dest, id);
  }

  return true;
}
