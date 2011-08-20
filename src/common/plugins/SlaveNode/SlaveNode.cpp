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
#include "ProxyAnonymousAuth.h"
#include "Settings.h"
#include "SlaveNode.h"
#include "Storage.h"

SlaveNode::SlaveNode(QObject *parent)
  : Core(parent)
  , m_mode(FailbackMode)
{
  qDebug() << "SLAVE NODE";

  addAuth(new ProxyAnonymousAuth(this));

  m_uplink = new AbstractClient(this);
  m_uplink->setNick(m_settings->value(QLatin1String("SlaveNode/Name"), QLatin1String("Slave")).toString());

  connect(m_uplink, SIGNAL(requestClientAuth()), SLOT(uplinkAuth()));
  connect(m_uplink, SIGNAL(packetReady(int)), SLOT(uplinkPacketReady(int)));
  connect(m_uplink, SIGNAL(ready()), SLOT(uplinkReady()));
  connect(m_uplink, SIGNAL(clientStateChanged(int, int)), SLOT(uplinkStateChanged(int)));
}


int SlaveNode::start()
{
  m_uplink->openUrl(m_settings->value(QLatin1String("SlaveNode/Url"), QString()).toString());
  return 0;
}


void SlaveNode::readPacket(int type)
{
  qDebug() << "SlaveNode::readPacket()" << type;
  if (mode() == ProxyMode) {
    if (type == Protocol::MessagePacket && readMessage()) {
      return;
    }
    else if (SimpleID::typeOf(m_reader->dest()) == SimpleID::UserId && m_storage->user(m_reader->dest())) {
      route();
      return;
    }
    else if (m_reader->headerOption() == Protocol::Broadcast)
      route();

    m_uplink->send(m_readBuffer);
  }
  else
    Core::readPacket(type);
}


bool SlaveNode::command()
{
  QString command = m_messageData->command;

//  if (command == QLatin1String("add")) {
//    if (SimpleID::isUserRoleId(m_reader->sender(), m_reader->dest()) && SimpleID::typeOf(m_reader->dest()) == SimpleID::TalksListId) {
//      bindTalks();
//    }
//
//    return false;
//  }

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
  else if (m_messageData->name) {
    rejectMessage(NoticeData::UnknownError);
    return true;
  }

  return false;
}


/*!
 * Отправка запроса авторизации на корневой сервер.
 */
void SlaveNode::uplinkAuth()
{
  AuthRequestData data(AuthRequestData::SlaveNode, m_uplink->url().host(), m_uplink->user().data());
  data.uniqueId = m_uplink->uniqueId();
  data.privateId = m_storage->serverData()->privateId();
  m_uplink->send(AuthRequestWriter(m_uplink->sendStream(), data).data());
}


void SlaveNode::uplinkPacketReady(int type)
{
  qDebug() << "";
  qDebug() << "UPLINK PACKET READY" << type;
  qDebug() << m_uplink->reader()->sender().toHex();
  qDebug() << m_uplink->reader()->dest().toHex();
  qDebug() << "";

  if (type == Protocol::AuthReplyPacket)
    uplinkAuthReply();
  else if (type == Protocol::MessagePacket)
    uplinkReadMessage();
  else if (type == Protocol::ChannelPacket)
    uplinkReadChannel();
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


void SlaveNode::uplinkStateChanged(int state)
{
  if (state != AbstractClient::ClientOnline)
    setMode(FailbackMode);
}


bool SlaveNode::uplinkRoute()
{
  int type = SimpleID::typeOf(m_uplink->reader()->dest());

  if (type == SimpleID::UserId)
    return uplinkRouteUser(m_uplink->reader()->dest());

  if (type == SimpleID::ChannelId)
    return uplinkRouteChannel(m_uplink->reader()->dest());

  if (m_uplink->reader()->headerOption() & Protocol::Broadcast)
    return uplinkRouteBroadcast();

  return false;
}


bool SlaveNode::uplinkRouteBroadcast()
{
  qDebug() << "";
  qDebug() << "ROUTE BROADCAST";
  qDebug() << "";
  ChatUser user = m_storage->user(m_uplink->reader()->sender());
  qDebug() << user;
  if (user)
    return true;

  m_broadcast = m_uplink->readBuffer();
//  qDebug() << m_storage->socketsFromUser(user);
//  return send(m_storage->socketsFromUser(user), m_uplink->readBuffer());
  return true;
}



bool SlaveNode::uplinkRouteChannel(const QByteArray &id)
{
  ChatChannel channel = m_storage->channel(id);
  if (!channel)
    return false;

  m_timestamp = m_uplink->timestamp();
  return send(m_storage->socketsFromChannel(channel), m_uplink->readBuffer());
}


bool SlaveNode::uplinkRouteUser(const QByteArray &id)
{
  ChatUser user = m_storage->user(id);
  if (!user)
    return false;

  m_timestamp = m_uplink->timestamp();
  return send(user, m_uplink->readBuffer());
}


ChatChannel SlaveNode::uplinkAddChannel()
{
  ChannelReader reader(m_uplink->reader());
  if (!reader.channel->isValid())
    return ChatChannel();

  ChatChannel channel = ChatChannel(new ServerChannel(ClientChannel(reader.channel)));
  m_storage->addChannel(channel);

  return channel;
}


void SlaveNode::setMode(Mode mode)
{
  m_mode = mode;
}


/*!
 * Получение ответа от корневого сервера на запрос авторизации локального клиента.
 */
void SlaveNode::uplinkAuthReply()
{
  ChatUser user = m_storage->user(m_uplink->reader()->dest());
  if (!user)
    return;

  qDebug() << "uplinkAuthReply()" << user;

  AuthReplyData data = AuthReplyReader(m_uplink->reader()).data;
  int option = 0;
  if (data.status == AuthReplyData::AccessGranted) {
    option = NewPacketsEvent::AuthorizeSocketOption;
    addChannel(user);
  }

  m_timestamp = m_uplink->timestamp();
  send(user, m_uplink->readBuffer(), option);

  if (data.status == AuthReplyData::AccessDenied && data.error != AuthReplyData::NickAlreadyUse)
    m_storage->remove(user);
}


/*!
 * Получение ответа от корневого сервера на запрос подключения к каналу.
 */
void SlaveNode::uplinkReadChannel()
{
  qDebug() << "SlaveNode::uplinkReadChannel()";
  ChatUser user = m_storage->user(m_uplink->reader()->dest());
  qDebug() << user;
  if (!user)
    return;

  ChatChannel channel = uplinkAddChannel();
  if (!channel)
    return;

  channel->addUser(user->id());
  user->addChannel(channel->id());

  QList<QByteArray> users = channel->users();
  for (int i = 0; i < users.size(); ++i) {
    ChatUser u = m_storage->user(users.at(i));
    if (!u)
      continue;

    u->addUser(user->id());
  }

  user->addUsers(users);
  uplinkRoute();
}


void SlaveNode::uplinkReadMessage()
{
  if (SimpleID::typeOf(m_uplink->reader()->dest()) == SimpleID::UserId) {
    if (!uplinkRouteUser(m_uplink->reader()->dest()))
      uplinkRejectMessage(MessageReader(m_uplink->reader()).data, NoticeData::UnknownError);

    return;
  }
  else if (m_uplink->reader()->headerOption() & Protocol::Broadcast) {
    if (m_storage->user(m_uplink->reader()->sender()))
      return;

    MessageData data = MessageReader(m_uplink->reader()).data;

    if (data.command == QLatin1String("x-broadcast")) {
      if (m_broadcast.isEmpty())
        return;

      QList<quint64> sockets;
      foreach(QByteArray id, m_uplink->reader()->idList()) {
        ChatUser u = m_storage->user(id);
        if (u && !sockets.contains(u->socketId()))
          sockets.append(u->socketId());
      }
      send(sockets, m_broadcast);
      m_broadcast.clear();
      return;
    }
  }

  uplinkRoute();
}


void SlaveNode::uplinkRejectMessage(const MessageData &msg, int reason)
{
  if (msg.name == 0)
    return;

  NoticeData data(m_uplink->reader()->dest(), m_uplink->reader()->sender(), NoticeData::MessageRejected, msg.name, reason);
  m_uplink->send(NoticeWriter(m_uplink->sendStream(), data).data());
}
