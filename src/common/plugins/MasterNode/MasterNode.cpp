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
#include "net/packets/notices.h"
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


/*!
 * \bug Эта функция больше не работает корректно в связи с тем что базовый класс теперь обрабатывает пакеты этого типа.
 */
bool MasterNode::readNotice()
{
  if (m_storage->isSlave(m_packetsEvent->userId()) && SimpleID::typeOf(m_reader->sender()) == SimpleID::ServerId) {
    quint16 type = m_reader->get<quint16>();

    if (type == AbstractNotice::TextNoticeType) {
      TextNotice notice(type, m_reader);
      if (notice.subtype() == TextNotice::SlaveNodeXHost) {
        m_hosts[m_reader->dest()] = notice.text();
      }
    }

    return true;
  }

  return route();
}


void MasterNode::acceptAuth(const AuthResult &result)
{
  ChatUser user = m_storage->user(result.id);
  if (!user)
    return;

  if (m_storage->isSlave(m_packetsEvent->userId())) {
    QByteArray id = result.id;
    if (!m_hosts.contains(id))
      id = m_storage->makeUserId(AuthRequest::Anonymous, user->uniqueId());

    if (m_hosts.contains(id)) {
      user->setHost(m_hosts.value(id));
      m_hosts.remove(id);
    }

    ChatUser slave = Storage::i()->user(m_packetsEvent->userId());
    if (slave)
      user->setServerNumber(slave->rawGender());

    AuthResult r = result;
    r.option = 0;
    Core::acceptAuth(r);
    return;
  }
  else {
    QList<QByteArray> slaves = m_storage->slaves();
    if (!slaves.isEmpty()) {
      UserWriter writer(m_sendStream, user.data(), slaves, user->cookie());
      send(m_storage->socketsFromIds(slaves), writer.data());
    }
  }

  Core::acceptAuth(result);
}


void MasterNode::readPacket(int type)
{
  if (type == Protocol::NoticePacket)
    readNotice();
  else
    Core::readPacket(type);
}


void MasterNode::rejectAuth(const AuthResult &result)
{
  if (m_storage->isSlave(m_packetsEvent->userId())) {
    AuthResult r = result;
    r.option = 0;
    Core::rejectAuth(r);
    return;
  }

  Core::rejectAuth(result);
}


void MasterNode::socketReleaseEvent(SocketReleaseEvent *event)
{
  if (m_storage->isSlave(event->userId())) {
    ChatUser slave = m_storage->user(event->userId());
    if (!slave)
      return;

    m_storage->remove(slave);
    m_storage->removeSlave(event->userId());

    quint8 number = slave->rawGender();
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

      m_storage->remove(users.at(i));
    }

    MessageData message(m_storage->serverData()->id(), channels, QLatin1String("split"), QString::number(number));
    send(m_storage->socketsFromIds(channels), MessageWriter(m_sendStream, message).data());
  }
  else
    Core::socketReleaseEvent(event);
}
