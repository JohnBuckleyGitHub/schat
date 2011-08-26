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
#include "net/packets/message.h"
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
    user->setServerNumber(data.serverData.number());

    setServerData(data.serverData);

    if (user->status() == User::OfflineStatus)
      user->setStatus(User::OnlineStatus);

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
  serverData->setNumber(data.number());

  setClientState(AbstractClient::ClientOnline);

  if (!sameServer)
    setup();
  else
    restore();
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


ClientUser AbstractClient::user() const
{
  Q_D(const AbstractClient);
  return d->user;
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
