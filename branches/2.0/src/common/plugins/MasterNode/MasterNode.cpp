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
#include "net/packets/message.h"
#include "net/ServerData.h"
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
  if (m_storage->isSlave(event->userId())) {
    ChatUser slave = m_storage->user(event->userId());
    if (!slave)
      return;

    quint8 number = slave->rawGender();
    QHash<QByteArray, ChatUser> all = m_storage->users();
    all.remove(event->userId());

    QList<ChatUser> users;

    // Поиск всех пользователей с вторичного сервера.
    QHashIterator<QByteArray, ChatUser> i(all);
    while (i.hasNext()) {
      i.next();
      if (i.value()->serverNumber() == number)
        users.append(i.value());
    }

    if (users.isEmpty())
      return;

    // Поиск всех каналов в которых находились пользователи со вторичного сервера.
    QList<QByteArray> channels;
    for (int i = 0; i < users.size(); ++i) {
      foreach (QByteArray id, users.at(i)->channels()) {
        if (!channels.contains(id))
          channels.append(id);
      }

      m_storage->remove(users.at(i));
    }

    MessageData message(m_storage->serverData()->id(), channels, QLatin1String("split"), QString::number(number));
    send(m_storage->socketsFromIds(channels), MessageWriter(m_sendStream, message).data());
  }
  else
    Core::socketReleaseEvent(event);
}
