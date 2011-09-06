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

#include "messages/UserMessage.h"
#include "net/packets/message.h"

UserMessage::UserMessage(int status, const MessageData &data)
  : AbstractMessage(QLatin1String("user-type"), data, NoParse)
  , m_status(static_cast<DeliveryStatus>(status))
{
  if (status & OutgoingMessage)
    m_direction = OutgoingDirection;
  else if (status & IncomingMessage)
    m_direction = IncomingDirection;

  if (data.flags & MessageData::OfflineFlag)
    m_timeTpl = QLatin1String("time-date");

  m_extra = extra(m_status);

  if (data.command.isEmpty())
    return;

  if (data.command == QLatin1String("me"))
    m_type = QLatin1String("action-type");
}


QString UserMessage::js(bool add) const
{
  if (add) {
    if (m_status & Rejected || m_status & Delivered)
      return setMessageState();

    if (m_text.isEmpty())
      return QString();
  }

  return AbstractMessage::js();
}


QString UserMessage::extra(DeliveryStatus status)
{
  QString out;
  if (status & OutgoingMessage) {
    out += QLatin1String(" outgoing");
  }
  else if (status & IncomingMessage) {
    out += QLatin1String(" incoming");
  }

  if (status & Undelivered)
    out += QLatin1String(" undelivered");

  if (status & Offline)
    out += QLatin1String(" offline");

  if (status & Rejected)
    out += QLatin1String(" rejected");
  else if (status & Delivered)
    out += QLatin1String(" delivered");

  return out;
}


QString UserMessage::setMessageState() const
{
  QDateTime dt = dateTime();

  return QString("setMessageState('#%1', '%2', '%3', '%4');")
      .arg(m_id)
      .arg(extra(m_status))
      .arg(dt.toString(QLatin1String("hh:mm")))
      .arg(dt.toString(QLatin1String(":ss")));
}
