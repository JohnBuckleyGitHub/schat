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
#include "net/packets/ChannelPacket.h"
#include "net/packets/messages.h"
#include "net/packets/Notice.h"
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
//  m_uplink->user()->gender().setRaw(m_settings->value(QLatin1String("SlaveNode/Number"), 1).toInt());
//  m_storage->serverData()->setNumber(m_uplink->user()->gender().raw());
}


int SlaveNode::start()
{
  m_plugins->removeHook(NodeHook::OfflineDelivery);
  m_uplink->openUrl(m_settings->value(QLatin1String("SlaveNode/Url"), QString()).toString());
  return 0;
}
