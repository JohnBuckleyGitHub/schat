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
#include "net/packets/message.h"
#include "net/packets/Notice.h"
#include "net/packets/users.h"
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


void MasterNode::accept(const AuthResult &result)
{
  ChatUser user = m_storage->user(result.id);
  if (!user)
    return;

  if (m_storage->isSlave(m_packetsEvent->channelId())) {
    if (m_hosts.contains(result.authId)) {
      user->setHost(m_hosts.value(result.authId));
      m_hosts.remove(result.authId);
    }

    ChatUser slave = Storage::i()->user(m_packetsEvent->channelId());
    if (slave)
      user->setServerNumber(slave->gender().raw());

    AuthResult r = result;
    r.option = 0;
    Core::accept(r);
    return;
  }
  else {
    QList<QByteArray> slaves = m_storage->slaves();
    if (!slaves.isEmpty()) {
      UserWriter writer(m_sendStream, user.data(), slaves, user->cookie());
      send(m_storage->sockets(slaves), writer.data());
    }
  }

  Core::accept(result);
}


void MasterNode::notice(quint16 type)
{
  if (m_storage->isSlave(m_packetsEvent->channelId()) && SimpleID::typeOf(m_reader->sender()) == SimpleID::ServerId) {
    quint16 type = m_reader->get<quint16>();

    if (type == Notice::GenericType) {
      Notice notice(type, m_reader);
      m_notice = &notice;

      if (!m_notice->isValid()) {
        rejectNotice(Notice::BadRequest);
        return;
      }

      if (m_notice->command() == "slave.user.host")
        m_hosts[m_reader->dest()] = notice.text();
    }
  }
  else
    Core::notice(type);
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


void MasterNode::release(SocketReleaseEvent *event)
{
  if (m_storage->isSlave(event->channelId())) {
    ChatUser slave = m_storage->user(event->channelId());
    if (!slave)
      return;

//    m_storage->remove(slave);
    m_storage->removeSlave(event->channelId());

    quint8 number = slave->gender().raw();
    QHash<QByteArray, ChatUser> all = m_storage->users();
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

//      m_storage->remove(users.at(i));
    }

    MessageData message(m_storage->serverData()->id(), channels, QLatin1String("split"), QString::number(number));
    send(m_storage->sockets(channels), MessageWriter(m_sendStream, message).data());
  }
  else
    Core::release(event);
}
