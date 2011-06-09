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
#include "ChatMessage.h"
#include "debugstream.h"
#include "net/packets/message.h"
#include "net/SimpleClient.h"
#include "schat2.h"
#include "User.h"

ChatMessage::ChatMessage()
  : m_status(UnknownStatus)
  , m_type(UnknownType)
  , m_parseOptions(NoParse)
  , m_timestamp(0)
  , m_name(0)
{
}


ChatMessage::ChatMessage(int status, const MessageData &data)
  : m_status(static_cast<DeliveryStatus>(status))
  , m_type(UserMessage)
  , m_parseOptions(RemoveUnSafeHtml)
  , m_destId(data.destId)
  , m_senderId(data.senderId)
  , m_timestamp(data.timestamp)
  , m_name(data.name)
{
  ChatUser user = ChatCore::i()->client()->user(data.senderId);
  if (!user)
    return;

  setNick(user->nick());
  setText(data.text);
}


void ChatMessage::setNick(const QString &nick)
{
  SCHAT_DEBUG_STREAM(this << "setNick()" << nick)
  m_nick = Qt::escape(nick);
}


QString ChatMessage::messageId() const
{
  return m_senderId.toHex() + "-" + QString::number(m_name);
}


void ChatMessage::setText(const QString &text)
{
  SCHAT_DEBUG_STREAM(this << "setText()" << text)
  m_text = text;
}
