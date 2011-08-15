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
#include "MasterAnonymousAuth.h"
#include "MasterNode.h"
#include "SlaveAuth.h"


MasterNode::MasterNode(QObject *parent)
  : Core(parent)
{
  qDebug() << "MASTER NODE";
  addAuth(new SlaveAuth(this));
  addAuth(new MasterAnonymousAuth(this));
}


void MasterNode::addSlave(const QByteArray &id)
{
  if (m_slaves.contains(id))
    return;

  m_slaves.append(id);
}


bool MasterNode::checkPacket()
{
  if (m_slaves.contains(m_packetsEvent->userId()))
    return true;

  return Core::checkPacket();
}


void MasterNode::socketReleaseEvent(SocketReleaseEvent *event)
{
  qDebug() << "SOCKET RELEASE";
  Core::socketReleaseEvent(event);
}
