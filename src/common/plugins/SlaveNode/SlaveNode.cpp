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
#include "ProxyAnonymousAuth.h"
#include "ServerChannel.h"
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
}


int SlaveNode::start()
{
  m_uplink->openUrl(m_settings->value(QLatin1String("SlaveNode/Url"), QString()).toString());
  return 0;
}


void SlaveNode::readPacket(int type)
{
  qDebug() << "SlaveNode::readPacket()" << type;
  if (mode() == ProxyMode)
    m_uplink->send(m_readBuffer);
  else
    Core::readPacket(type);
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

  ChannelReader reader(m_uplink->reader());
  if (!reader.channel->isValid())
    return;

  ServerChannel *channel = this->channel(reader.channel->name());
  if (!channel)
    return;

  if (channel->id() != reader.channel->id())
    return;

  channel->addUser(user->id());
  user->addId(SimpleID::ChannelListId, channel->id());

  QList<QByteArray> users = reader.channel->users();
  for (int i = 0; i < users.size(); ++i) {
    ChatUser u = m_storage->user(users.at(i));
    if (!u)
      continue;

    u->addUser(user->id());
  }

  uplinkRoute();
}


void SlaveNode::uplinkRoute()
{
  int type = SimpleID::typeOf(m_uplink->reader()->dest());

  if (type == SimpleID::UserId)
    uplinkRouteUser(m_uplink->reader()->dest());
  else if (type == SimpleID::ChannelId)
    uplinkRouteChannel(m_uplink->reader()->dest());
}



void SlaveNode::uplinkRouteChannel(const QByteArray &id)
{
  ServerChannel *channel = m_storage->channel(id);
  if (!channel)
    return;

  m_timestamp = m_uplink->timestamp();
  send(m_storage->socketsFromChannel(channel), m_uplink->readBuffer());
}


void SlaveNode::uplinkRouteUser(const QByteArray &id)
{
  ChatUser user = m_storage->user(id);
  if (!user)
    return;

  m_timestamp = m_uplink->timestamp();
  send(user, m_uplink->readBuffer());
}
