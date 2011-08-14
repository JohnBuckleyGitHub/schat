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
#include "ProxyAnonymousAuth.h"
#include "Settings.h"
#include "SlaveNode.h"
#include "Storage.h"

SlaveNode::SlaveNode(QObject *parent)
  : GenericCore(parent)
  , m_mode(FailbackMode)
{
  qDebug() << "SLAVE NODE";

  m_auth.prepend(new ProxyAnonymousAuth(this));

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
  m_uplink->send(m_readBuffer);
}


/*!
 * Отправка запроса авторизации.
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
  qDebug() << "";

  if (type == Protocol::AuthReplyPacket)
    uplinkAuthReply();
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


void SlaveNode::uplinkRoute()
{
  qDebug() << m_uplink->reader()->sender().toHex();
  qDebug() << m_uplink->reader()->dest().toHex();
}
