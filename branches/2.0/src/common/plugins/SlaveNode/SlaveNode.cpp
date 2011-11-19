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

#include <QDebug>

#include "client/AbstractClient.h"
#include "events.h"
#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "net/packets/channels.h"
#include "net/packets/message.h"
#include "net/packets/notices.h"
#include "net/packets/users.h"
#include "NodePlugins.h"
#include "Settings.h"
#include "SlaveAnonymousAuth.h"
#include "SlaveCookieAuth.h"
#include "SlaveNode.h"
#include "Storage.h"

SlaveNode::SlaveNode(QObject *parent)
  : Core(parent)
  , m_mode(FailbackMode)
{
  qDebug() << "SLAVE NODE";

  qDeleteAll(m_auth);
  m_auth.clear();

  addAuth(new SlaveAnonymousAuth(this));
  addAuth(new BypassSlaveCookieAuth(this));
  addAuth(new SlaveCookieAuth(this));

  m_uplink = new AbstractClient(this);
  m_uplink->setNick(m_settings->value(QLatin1String("SlaveNode/Name"), QLatin1String("Slave")).toString());
  m_uplink->user()->gender().setRaw(m_settings->value(QLatin1String("SlaveNode/Number"), 1).toInt());
  m_storage->serverData()->setNumber(m_uplink->user()->gender().raw());

  connect(m_uplink, SIGNAL(requestClientAuth()), SLOT(uplinkAuth()));
  connect(m_uplink, SIGNAL(packetReady(int)), SLOT(uplinkPacketReady(int)));
  connect(m_uplink, SIGNAL(ready()), SLOT(uplinkReady()));
  connect(m_uplink, SIGNAL(clientStateChanged(int, int)), SLOT(uplinkStateChanged(int, int)));
}


//bool SlaveNode::add(ChatUser user, int authType, const QByteArray &authId)
//{
//  if (mode() == FailbackMode) {
//    if (authType == AuthRequest::Anonymous)
//      return false;
//
//    return Core::add(user, authType, authId);
//  }
//
//  m_pending[authId] = user;
//
//  QList<QByteArray> packets;
//  Notice notice(Storage::i()->serverData()->id(), authId, "slave.user.host");
//  notice.setText(m_packetsEvent->address.toString());
//
//  packets.append(notice.data(uplink()->sendStream()));
//
//  packets.append(m_readBuffer);
//  m_uplink->send(packets);
//
//  return true;
//}


int SlaveNode::start()
{
  m_plugins->removeHook(NodeHook::OfflineDelivery);
  m_uplink->openUrl(m_settings->value(QLatin1String("SlaveNode/Url"), QString()).toString());
  return 0;
}


void SlaveNode::readPacket(int type)
{
  qDebug() << "SlaveNode::readPacket()" << type;
  if (mode() == ProxyMode) {
    if (type == Protocol::MessagePacket) {
      if (readMessage())
        return;
    }
    else if (type == Protocol::UserDataPacket) {
      readUserData();
    }
    else if (SimpleID::typeOf(m_reader->dest()) == SimpleID::UserId && m_storage->user(m_reader->dest())) {
      route();
      return;
    }

    m_uplink->send(m_readBuffer);
  }
  else
    Core::readPacket(type);
}


void SlaveNode::release(SocketReleaseEvent *event)
{
  ChatUser user = m_storage->user(event->userId());
  if (!user)
    return;

  if (mode() == ProxyMode) {
    QByteArray packet = MessageWriter(m_sendStream, MessageData(user->id(), user->channels(), QLatin1String("leave"), QString())).data();
    send(m_storage->sockets(user->channels()), packet);
    m_uplink->send(packet);
    m_storage->remove(user);
  }
  else
    Core::release(event);
}


bool SlaveNode::command()
{
  QString command = m_messageData->command;

  if (command == QLatin1String("status")) {
    updateUserStatus();
    return false;
  }

  return false;
}


/*!
 * Обработка сообщений от локальных пользователей.
 *
 * \return true если сообщение обработано и дальнейшая его пересылка не требуется.
 */
bool SlaveNode::readMessage()
{
  qDebug() << "SlaveNode::readMessage()";

  // Сообщение для неизвестного пользователя перенаправляется на мастер-сервер.
  if (SimpleID::typeOf(m_reader->dest()) == SimpleID::UserId && !m_storage->user(m_reader->dest())) {
    return false;
  }

  MessageReader reader(m_reader);
  m_messageData = &reader.data;

  if (m_messageData->options & MessageData::ControlOption && command())
    return true;

  if (route()) {
    acceptMessage();
    return false;
  }
  else {
    rejectMessage(Notice::InternalError);
    return true;
  }

  return false;
}


/*!
 * Если пользователь изменил ник, вторичный сервер отправляет данные
 * на корневой сервер и не применяет изменения, т.к. корневой сервер может
 * отклонить пакет из-за конфликта ников.
 * Вторичный сервер самостоятельно отследить коллизию ников не может.
 */
bool SlaveNode::readUserData()
{
  ChatUser user = m_storage->user(m_reader->sender());
  if (!user)
    return false;

  UserReader reader(m_reader);
  if (user->nick() != reader.user.nick())
    return false;

  if (updateUserData(user, &reader.user)) {
    route();
    return true;
  }

  return false;
}


/*!
 * Отправка запроса авторизации на корневой сервер.
 */
void SlaveNode::uplinkAuth()
{
  AuthRequest data(AuthRequest::SlaveNode, m_uplink->url().host(), m_uplink->user().data());
  data.uniqueId = m_uplink->uniqueId();
  data.privateId = m_storage->serverData()->privateId();
  m_uplink->send(data.data(m_uplink->sendStream()));
}


void SlaveNode::uplinkPacketReady(int type)
{
  qDebug() << "";
  qDebug() << "UPLINK PACKET READY" << type;
  qDebug() << SimpleID::encode(m_uplink->reader()->sender());
  qDebug() << SimpleID::encode(m_uplink->reader()->dest());
  qDebug() << "";

  if (type == Protocol::AuthReplyPacket)
    uplinkAuthReply();
  else if (type == Protocol::MessagePacket)
    uplinkReadMessage();
  else if (type == Protocol::UserDataPacket)
    uplinkReadUserData();
//  else if (type == Protocol::ChannelPacket)
//    uplinkReadChannel();
  else
    uplinkRoute();
}


void SlaveNode::uplinkReady()
{
  setMode(ProxyMode);
  qDebug() << "";
  qDebug() << "UPLINK READY";
  qDebug() << "";
}


void SlaveNode::uplinkStateChanged(int state, int previousState)
{
  if (state != AbstractClient::ClientOnline)
    setMode(FailbackMode);

  if (previousState == AbstractClient::ClientOnline)
    split();
}


void SlaveNode::reAuth()
{
  qDebug() << "-------------------";
  qDebug() << "RE AUTH" << m_storage->users().size();
  qDebug() << "-------------------";

  QHash<QByteArray, ChatUser> users = m_storage->users();
  if (users.isEmpty())
    return;

  Notice notice(Storage::i()->serverData()->id(), QByteArray(), "slave.user.host");
  QList<QByteArray> packets;

  QHashIterator<QByteArray, ChatUser> i(users);
  while (i.hasNext()) {
    i.next();
    if (!i.value()->isOnline())
      continue;

    notice.setDest(SimpleID::randomId(SimpleID::MessageId));
    notice.setText(i.value()->host());
    packets.append(notice.data(m_uplink->sendStream()));

    AuthRequest data(AuthRequest::Cookie, QString(), i.value().data());
    data.uniqueId = i.value()->uniqueId();
    data.cookie = i.value()->cookie();
    data.id = notice.dest();

    packets.append(data.data(m_uplink->sendStream()));
  }

  m_uplink->send(packets);
}


void SlaveNode::split()
{
//  QList<QByteArray> channels = m_storage->channels().keys();
//  MessageData message(m_storage->serverData()->id(), channels, QLatin1String("split"), QString());
//
//  send(m_storage->sockets(channels), MessageWriter(m_sendStream, message).data());
}


bool SlaveNode::uplinkRoute()
{
  m_timestamp = m_uplink->timestamp();
  return send(m_storage->sockets(m_uplink->reader()->destinations()), m_uplink->readBuffer());
}


bool SlaveNode::uplinkRouteUser(const QByteArray &id)
{
  ChatUser user = m_storage->user(id);
  if (!user)
    return false;

  m_timestamp = m_uplink->timestamp();
  return send(user, m_uplink->readBuffer());
}


/*!
 * \deprecated Эта функция больше не работает.
 */
ChatChannel SlaveNode::uplinkAddChannel()
{
//  ChannelReader reader(m_uplink->reader());
//  if (!reader.channel->isValid())
//    return ChatChannel();
//
//  ChatChannel channel = ChatChannel(new ServerChannel(ClientChannel(reader.channel)));
//  m_storage->addChannel(channel);

  return ChatChannel();
}


void SlaveNode::setMode(Mode mode)
{
  m_mode = mode;

  if (mode == ProxyMode)
    reAuth();
  else
    m_uplink->setAuthorized(QByteArray());
}


/*!
 * Получение ответа от корневого сервера на запрос авторизации локального клиента.
 */
void SlaveNode::uplinkAuthReply()
{
  AuthReply data(m_uplink->reader());

  bool update = false;
  ChatUser user = m_storage->user(data.userId);
  if (user) {
    update = true;
  }
  else {
    user = m_pending.value(data.id);

    if (!user)
      return;
  }

  QByteArray id = user->id();
  int option = 0;

  if (data.status == Notice::OK) {
    option = NewPacketsEvent::AuthorizeSocketOption;
    user->setId(data.userId);
    user->setCookie(data.cookie);

//    if (update)
//      m_storage->update(user);
//    else
//      m_storage->add(user); /// \bug Добавление пользователя больше не работает.

//    addChannel(user);
  }
  else if (data.status != Notice::NickAlreadyUse) {
    option = NewPacketsEvent::KillSocketOption;
    m_storage->remove(user);
    m_pending.remove(data.id);
  }

  m_timestamp = m_uplink->timestamp();
  send(user, m_uplink->readBuffer(), option);
}


/*!
 * Получение ответа от корневого сервера на запрос подключения к каналу.
 */
void SlaveNode::uplinkReadChannel()
{
  qDebug() << "SlaveNode::uplinkReadChannel()";
  ChatUser user = m_storage->user(m_uplink->reader()->dest());
  if (!user)
    return;

  ChatChannel channel = uplinkAddChannel();
  if (!channel)
    return;

  channel->channels().add(user->id());
  user->addChannel(channel->id());

  uplinkRoute();
}


void SlaveNode::uplinkReadMessage()
{
  if (SimpleID::typeOf(m_uplink->reader()->dest()) == SimpleID::UserId) {
    if (!uplinkRouteUser(m_uplink->reader()->dest()))
      uplinkRejectMessage(MessageReader(m_uplink->reader()).data, Notice::InternalError);

    return;
  }

  uplinkRoute();
}


void SlaveNode::uplinkReadUserData()
{
  ChatUser user = m_storage->user(m_uplink->reader()->sender());
  if (user) {
    UserReader reader(m_uplink->reader());
    if (user->nick() == reader.user.nick() && !(reader.fields & UserWriter::StaticData))
      return;

    updateUserData(user, &reader.user);
  }
  else if (m_uplink->reader()->sender() != m_uplink->userId() && m_uplink->reader()->destinations().contains(m_uplink->userId())) {
    UserReader reader(m_uplink->reader());
    if (reader.user.isValid()) {
      ChatUser user(new ServerUser(&reader.user));
      user->setCookie(reader.cookie);
      user->setNormalNick(m_storage->normalize(user->nick()));
      m_storage->store(user);
    }
  }

  uplinkRoute();
}


/*!
 * \deprecated Эту функцию необходимо переписать, т.к. изменён способ уведомления об отклонении сообщений.
 */
void SlaveNode::uplinkRejectMessage(const MessageData &msg, int reason)
{
  Q_UNUSED(msg)
  Q_UNUSED(reason)
//  if (msg.id.isEmpty())
//    return;

//  MessageNotice notice(MessageNotice::Rejected, m_uplink->reader()->dest(), m_uplink->reader()->sender(), m_messageData->id, reason);
//  m_uplink->send(notice.data(m_sendStream));
}
