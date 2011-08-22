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
#include "Storage.h"

MasterNode::MasterNode(QObject *parent)
  : Core(parent)
{
  qDebug() << "MASTER NODE";
  m_storage->setAllowSlaves();

  addAuth(new SlaveAuth(this));
  addAuth(new MasterAnonymousAuth(this));
}


void MasterNode::socketReleaseEvent(SocketReleaseEvent *event)
{
//  if (m_storage->isSlave(event->userId())) {
//    qDebug() << "";
//    qDebug() << "";
//    qDebug() << "SLAVE LEAVE";
//    qDebug() << "";
//    ChatUser slave = m_storage->user(event->userId());
//    if (!slave)
//      return;
//
//    quint64 socket = slave->socketId();
//    QHash<QByteArray, ChatUser> all = m_storage->users();
//    all.remove(event->userId());
//
//    QList<ChatUser> users;
//
//    QHashIterator<QByteArray, ChatUser> i(all); // Поиск всех пользователей с вторичного сервера.
//    while (i.hasNext()) {
//      i.next();
//      if (i.value()->socketId() == socket)
//        users.append(i.value());
//    }
//
//    qDebug() << "USERS TOTAL:" << users.size();
//  }
//  else
    Core::socketReleaseEvent(event);
}
