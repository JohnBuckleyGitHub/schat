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

#include "cores/SlaveNode.h"
#include "client/AbstractClient.h"

SlaveNode::SlaveNode(QObject *parent)
  : GenericCore(parent)
{
  qDebug() << "SLAVE NODE";

  m_uplink = new AbstractClient(this);
  m_uplink->setNick("Slave");

  connect(m_uplink, SIGNAL(requestClientAuth()), SLOT(uplinkAuth()));
  connect(m_uplink, SIGNAL(ready()), SLOT(uplinkReady()));
}


int SlaveNode::start()
{
  m_uplink->openUrl("schat://localhost:7668");
  return 0;
}


void SlaveNode::uplinkAuth()
{
  qDebug() << "";
  qDebug() << "UPLINK AUTH";
  qDebug() << "";
}


void SlaveNode::uplinkReady()
{
  qDebug() << "";
  qDebug() << "UPLINK READY";
  qDebug() << "";
}


void SlaveNode::acceptAuth(ChatUser user)
{
  qDebug() << this << "acceptAuth()";
  GenericCore::acceptAuth(user);
  m_uplink->send(m_readBuffer);
}
