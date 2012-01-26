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

#include "events.h"
#include "MasterNode.h"
#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "net/packets/Notice.h"
#include "net/ServerData.h"
#include "SlaveAuth.h"
#include "Storage.h"

MasterNode::MasterNode(QObject *parent)
  : Core(parent)
{
  qDebug() << "MASTER NODE";
  m_storage->setAllowSlaves();

  addAuth(new SlaveAuth(this));
}


void MasterNode::reject(const AuthResult &result)
{
  if (m_storage->isSlave(m_packetsEvent->channelId())) {
    AuthResult r = result;
    r.option = 0;
    Core::reject(r);
    return;
  }

  Core::reject(result);
}
