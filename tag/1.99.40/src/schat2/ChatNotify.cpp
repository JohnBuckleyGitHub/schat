/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include <QTimer>

#include "ChatNotify.h"
#include "net/packets/FeedNotice.h"
#include "sglobal.h"

ChatNotify *ChatNotify::m_self = 0;


FeedNotify::FeedNotify(const QByteArray &channel, const FeedNotice *packet)
  : Notify(FeedReply)
  , m_status(packet->status())
  , m_channel(channel)
  , m_command(packet->command())
  , m_name(packet->text())
  , m_json(packet->json())
{
}


FeedNotify::FeedNotify(int type, const QByteArray &channel, const QString &name, const QVariantMap &json, int status)
  : Notify(type)
  , m_status(status)
  , m_channel(channel)
  , m_name(name)
  , m_json(json)
{
}


bool FeedNotify::match(const QByteArray &id, const QString &name, const QString &action) const
{
  if (m_channel != id)
    return false;

  return match(name, action);
}


bool FeedNotify::match(const QString &name, const QString &action) const
{
  if (m_name != name)
    return false;

  if (action.isEmpty())
    return true;

  if (this->action() != action)
    return false;

  return true;
}


/*!
 * \deprecated Использование "action" устарело.
 */
QString FeedNotify::action() const
{
  return m_json.value(LS("action")).toString();
}


ChatNotify::ChatNotify(QObject *parent)
  : QObject(parent)
{
  m_self = this;
}


/*!
 * Немедленная отправка уведомления.
 */
void ChatNotify::start(const Notify &notify)
{
  emit m_self->notify(notify);
}


void ChatNotify::start(int type, const QVariant &data, bool queued)
{
  if (queued)
    start(new Notify(type, data));
  else
    start(Notify(type, data));
}


/*!
 * Отложенная отправка уведомления.
 */
void ChatNotify::start(Notify *notify)
{
  m_self->m_queue.enqueue(NotifyPtr(notify));
  if (m_self->m_queue.size() == 1)
    QTimer::singleShot(0, m_self, SLOT(start()));
}


void ChatNotify::start()
{
  while (!m_queue.isEmpty())
    emit notify(*m_queue.dequeue());
}


void ChatNotify::startNotify(const Notify &notify)
{
  emit this->notify(notify);
}
