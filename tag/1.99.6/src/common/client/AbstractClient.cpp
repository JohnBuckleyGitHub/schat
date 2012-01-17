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

#include <QBasicTimer>
#include <QTimerEvent>

#include "Account.h"
#include "Channel.h"
#include "client/AbstractClient.h"
#include "client/AbstractClient_p.h"
#include "client/NetworkPool.h"
#include "debugstream.h"
#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "net/packets/Notice.h"
#include "net/PacketWriter.h"
#include "net/Protocol.h"

AbstractClientPrivate::AbstractClientPrivate()
  : clientState(AbstractClient::ClientOffline)
  , previousState(AbstractClient::ClientOffline)
  , sendLock(false)
  , channel(new Channel())
  , server(new Channel())
  , collisions(0)
  , reconnects(0)
  , reconnectTimer(new QBasicTimer())
  , uniqueId(SimpleID::uniqueId())
  , serverData(new ServerData())
{
  Account account;
  channel->setAccount(&account);
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
  collisions++;

  int size = 1;
  if (collisions > 10 && collisions <= 15)
    size = 2;
  else if (collisions > 15)
    size = 3;

  int rand = 0;
  if (size == 1)
    rand = qrand() % 9;
  else if (size == 2)
    rand = qrand() % 89 + 10;
  else if (size == 3)
    rand = qrand() % 899 + 100;

  if (nick.isEmpty())
    nick = channel->name();

  return nick.left(Channel::MaxNameLength - size) + QString::number(rand);
}


/*!
 * Чтение пакета Protocol::AuthReplyPacket.
 */
bool AbstractClientPrivate::authReply(const AuthReply &reply)
{
  SCHAT_DEBUG_STREAM(this << "AuthReply" << reply.status << Notice::status(reply.status) << SimpleID::encode(reply.userId))

  Q_Q(AbstractClient);
  if (reply.status == Notice::OK) {
    collisions = 0;
    q->setAuthorized(reply.userId);
    channel->setId(reply.userId);
    channel->account()->setName(reply.account);
    channel->account()->setCookie(reply.cookie);

    cookie = reply.cookie;
    pool->setLast();
    authId.clear();

    if (channel->status().value() == Status::Offline)
      channel->status().set(Status::Online);

    setServerData(reply.serverData);

    emit(q->ready());
    return true;
  }

  if (reply.status == Notice::NickAlreadyUse) {
    authId = reply.id;

    if (collisions > 19) {
      setClientState(AbstractClient::ClientError);
      return false;
    }

    channel->setName(mangleNick());
    q->requestAuth();
    return false;
  }

  collisions = 0;
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

  server->setId(data.id());
  server->setName(data.name());
  serverData->setId(data.id());
  serverData->setName(data.name());
  serverData->setChannelId(data.channelId());
  serverData->setFeatures(data.features());
  serverData->setNumber(data.number());

  setClientState(AbstractClient::ClientOnline);

  Q_Q(AbstractClient);
  if (!sameServer)
    emit(q->setup());
  else
    emit(q->restore());
}


/*!
 * Запуск таймера повторного подключения.
 */
void AbstractClientPrivate::startReconnectTimer()
{
  if (reconnectTimer->isActive())
    reconnectTimer->stop();

  Q_Q(AbstractClient);
  if (reconnects < pool->count() + Protocol::MaxFastReconnects) {
    reconnectTimer->start(Protocol::FastReconnectTime, q);
  }
  else if (reconnects < pool->count() + Protocol::MaxFastReconnects + Protocol::MaxNormalReconnects) {
    reconnectTimer->start(Protocol::NormalReconnectTime, q);
  }
  else {
    reconnectTimer->start(Protocol::SlowReconnectTime, q);
  }

  ++reconnects;
}


AbstractClient::AbstractClient(QObject *parent)
  : SimpleSocket(*new AbstractClientPrivate(), parent)
{
  connect(this, SIGNAL(requestAuth(quint64)), SLOT(requestAuth()));
  connect(this, SIGNAL(released(quint64)), SLOT(released()));

  Q_D(AbstractClient);
  d->pool = new NetworkPool(this);
}


AbstractClient::AbstractClient(AbstractClientPrivate &dd, QObject *parent)
  : SimpleSocket(dd, parent)
{
  connect(this, SIGNAL(requestAuth(quint64)), SLOT(requestAuth()));
  connect(this, SIGNAL(released(quint64)), SLOT(released()));

  Q_D(AbstractClient);
  d->pool = new NetworkPool(this);
}


AbstractClient::~AbstractClient()
{
}


/*!
 * Установка подключения к серверу.
 */
bool AbstractClient::openUrl(const QUrl &url, const QByteArray &cookie, OpenOptions options)
{
  SCHAT_DEBUG_STREAM(this << "openUrl()" << url.toString())
  Q_D(AbstractClient);

  d->cookie = cookie;

  if (options & SaveUrl)
    d->url = url;

  if (!url.isValid())
    return false;

  if (url.scheme() != QLatin1String("schat"))
    return false;

  if (d->reconnectTimer->isActive())
    d->reconnectTimer->stop();

  if (state() != QAbstractSocket::UnconnectedState)
    leave();

  if (!d->nick.isEmpty())
    d->channel->setName(d->nick);

  d->setClientState(ClientConnecting);

  if (d->pool->open(url))
    return true;

  if (options & SaveUrl)
    d->pool->reset();

  connectToHost(url.host(), url.port(Protocol::DefaultPort));
  return true;
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


bool AbstractClient::send(Packet packet, bool echo)
{
  Q_D(AbstractClient);
  return send(packet->data(d->sendStream, echo));
}


ClientChannel AbstractClient::channel() const
{
  Q_D(const AbstractClient);
  return d->channel;
}


ClientChannel AbstractClient::server() const
{
  Q_D(const AbstractClient);
  return d->server;
}


AbstractClient::ClientState AbstractClient::clientState() const
{
  Q_D(const AbstractClient);
  return d->clientState;
}


AbstractClient::ClientState AbstractClient::previousState() const
{
  Q_D(const AbstractClient);
  return d->previousState;
}


const QByteArray& AbstractClient::cookie() const
{
  Q_D(const AbstractClient);
  return d->cookie;
}


const QByteArray& AbstractClient::uniqueId() const
{
  Q_D(const AbstractClient);
  return d->uniqueId;
}


/*!
 * Получение оригинального ника, не искажённого функцией
 * автоматического разрешения коллизий.
 */
const QString& AbstractClient::nick() const
{
  Q_D(const AbstractClient);
  if (d->nick.isEmpty())
    return d->channel->name();

  return d->nick;
}


const QUrl& AbstractClient::url() const
{
  Q_D(const AbstractClient);
  return d->url;
}


PacketReader *AbstractClient::reader()
{
  Q_D(const AbstractClient);
  return d->reader;
}


//QByteArray AbstractClient::serverId() const
//{
//  Q_D(const AbstractClient);
//  return d->serverData->id();
//}


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
  d->channel->setName(nick);
  d->nick = QString();
}


void AbstractClient::setUniqueId(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::UniqueUserId)
    return;

  Q_D(AbstractClient);
  d->uniqueId = id;
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

  d->date = 0;
  d->setClientState(ClientOffline);
  SimpleSocket::leave();
  setAuthorized(QByteArray());
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

    if (isAuthorized()) {
      emit packetReady(reader.type());
    }
    else if (reader.type() == Protocol::AuthReplyPacket) {
      d->authReply(AuthReply(d->reader));
    }
  }

  emit idle();
}


void AbstractClient::timerEvent(QTimerEvent *event)
{
  Q_D(AbstractClient);
  if (event->timerId() == d->reconnectTimer->timerId()) {
    QUrl url;
    if (d->reconnects <= Protocol::MaxFastReconnects && d->pool->hasLast())
      url = d->pool->last();
    else
      url = d->pool->next();

    if (url.isEmpty())
      url = d->url;

    openUrl(url, d->cookie, NoOptions);
    return;
  }

  SimpleSocket::timerEvent(event);
}


void AbstractClient::requestAuth()
{
  emit requestClientAuth();
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

  if (d->clientState == ClientOffline || d->clientState == ClientError)
    return;

  if (d->clientState == ClientOnline) {
    d->setClientState(ClientOffline);
    d->setClientState(ClientConnecting);
  }

  d->startReconnectTimer();
}
