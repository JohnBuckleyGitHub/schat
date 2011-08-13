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
#include "cores/SlaveNode.h"
#include "net/packets/auth.h"
#include "Settings.h"
#include "Storage.h"

SlaveNode::SlaveNode(QObject *parent)
  : GenericCore(parent)
{
  qDebug() << "SLAVE NODE";

  m_uplink = new AbstractClient(this);
  m_uplink->setNick(m_settings->value(QLatin1String("SlaveNode/Name"), QLatin1String("Slave")).toString());

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

  AuthRequestData data(AuthRequestData::SlaveNode, m_uplink->url().host(), m_uplink->user().data());
  data.uniqueId = m_uplink->uniqueId();
  data.privateId = m_storage->serverData()->privateId();
  m_uplink->send(AuthRequestWriter(m_uplink->sendStream(), data).data());
}


void SlaveNode::uplinkReady()
{
  qDebug() << "";
  qDebug() << "UPLINK READY";
  qDebug() << "";
}
