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

#include <QTextDocument>

#include "ChatCore.h"
#include "debugstream.h"
#include "messages/UserMessage.h"
#include "net/packets/message.h"
#include "net/SimpleClient.h"
#include "schat2.h"
#include "User.h"

UserMessage::UserMessage(int status, const MessageData &data)
  : AbstractMessage(UserMessageType, data.text, data.destId, NoParse)
  , m_status(static_cast<DeliveryStatus>(status))
  , m_name(data.name)
{
  ChatUser user = ChatCore::i()->client()->user(data.senderId);
  if (!user)
    return;

  m_senderId = data.senderId;
  m_timestamp = data.timestamp;

  setNick(user->nick());
}


QString UserMessage::js() const
{
  if (m_status & Rejected)
    return setMessageState("rejected");

  if (m_status & Delivered)
    return setMessageState("delivered");

  if (m_text.isEmpty())
    return "";

  QString html = tpl("message");
  replaceTimeStamp(html);
  replaceText(html);

  QString extra;
  if (m_status & UserMessage::IncomingMessage)
    extra += " incoming";

  if (m_status & UserMessage::OutgoingMessage)
    extra += " outgoing";

  if (m_status & UserMessage::Undelivered)
    extra += " undelivered";

  html.replace("%nick%", nick());
  html.replace("%extra%", extra);
  html.replace("%userId%", m_senderId.toHex());
  html.replace("%messageId%", messageId());
  html.replace("%messageType%", "user-message");

  return appendMessage(html);
}


QString UserMessage::messageId() const
{
  return m_senderId.toHex() + "-" + QString::number(m_name);
}


void UserMessage::setNick(const QString &nick)
{
  m_nick = Qt::escape(nick);
}


QString UserMessage::setMessageState(const QString &state) const
{
  QDateTime dt = dateTime();

  return QString("setMessageState('#%1', '%2', '%3', '%4');")
      .arg(messageId())
      .arg(state)
      .arg(dt.toString("hh:mm"))
      .arg(dt.toString(":ss"));
}
