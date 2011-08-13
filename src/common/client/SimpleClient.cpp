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

#include "debugstream.h"

#include "Channel.h"
#include "client/SimpleClient.h"
#include "client/SimpleClient_p.h"
#include "net/packets/auth.h"
#include "net/packets/message.h"

SimpleClientPrivate::SimpleClientPrivate()
{
}


SimpleClientPrivate::~SimpleClientPrivate()
{
}


/*!
 * Очистка данных состояния клиента.
 */
void SimpleClientPrivate::clearClient()
{
  user->remove(SimpleID::ChannelListId);
  user->remove(SimpleID::TalksListId);

  users.clear();
  users.insert(user->id(), user);

  qDeleteAll(channels);
  channels.clear();
}


/*!
 * Восстановление состояния клиента после повторного подключения к предыдущему серверу.
 */
void SimpleClientPrivate::restore()
{
  Q_Q(SimpleClient);
  q->lock();

  /// Происходит восстановление приватных разговоров.
  if (user->count(SimpleID::TalksListId)) {
    MessageData data(userId, SimpleID::setType(SimpleID::TalksListId, userId), QLatin1String("add"), QString());
    MessageWriter writer(sendStream, data);
    writer.putId(user->ids(SimpleID::TalksListId));
    q->send(writer.data());
  }

  /// Клиент заново входит в ранее открытие каналы.
  if (!channels.isEmpty()) {
    MessageData data(userId, QByteArray(), QLatin1String("join"), QString());
    data.options |= MessageData::TextOption;

    QHashIterator<QByteArray, Channel*> i(channels);
    while (i.hasNext()) {
      i.next();
      data.destId = i.key();
      data.text = i.value()->name();
      q->send(MessageWriter(sendStream, data).data());
    }
  }

  clearClient();
  q->unlock();
}


void SimpleClientPrivate::setup()
{
  clearClient();

  Q_Q(SimpleClient);
  if (serverData->features() & ServerData::AutoJoinSupport && !serverData->channelId().isEmpty()) {
    MessageData data(user->id(), serverData->channelId(), QLatin1String("join"), QString());
    q->send(MessageWriter(sendStream, data).data());
  }
}


SimpleClient::SimpleClient(QObject *parent)
  : AbstractClient(*new SimpleClientPrivate(), parent)
{
}


SimpleClient::SimpleClient(SimpleClientPrivate &dd, QObject *parent)
  : AbstractClient(dd, parent)
{
}


SimpleClient::~SimpleClient()
{
}


void SimpleClient::requestAuth()
{
  SCHAT_DEBUG_STREAM(this << "requestAuth()")

  Q_D(SimpleClient);

  AuthRequestData data(AuthRequestData::Anonymous, d->url.host(), d->user.data());
  data.uniqueId = d->uniqueId;
  send(AuthRequestWriter(d->sendStream, data).data());
}
