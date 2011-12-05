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

#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "messages/ChannelMessage.h"
#include "net/packets/MessagePacket.h"
#include "net/SimpleID.h"

ChannelMessage::ChannelMessage(const MessagePacket &packet)
  : Message()
  , m_packet(packet)
{
  m_data["Type"]      = "channel";
  m_data["Id"]        = QString(SimpleID::encode(packet.id()));
  m_data["Text"]      = packet.text();
  m_data["Direction"] = m_packet.sender() == ChatClient::id() ? "outgoing" : "incoming";
  m_data["Date"]      = m_packet.date();
//  m_data["Day"]       = true; // true включает отображение полного времени.

  ClientChannel user = ChatClient::channels()->get(m_packet.sender());
  if (user) {
    QVariantMap author;
    author["Id"]      = QString(SimpleID::encode(user->id()));
    author["Name"]    = Qt::escape(user->name());
    author["Url"]     = ChatUrls::toUrl(user, "insert").toString();
    m_data["Author"]  = author;
  }

  m_tab = detectTab();
}


QByteArray ChannelMessage::detectTab() const
{
  if (SimpleID::typeOf(m_packet.dest()) == SimpleID::ChannelId)
    return m_packet.dest();

  if (m_packet.sender() == ChatClient::id())
    return m_packet.dest();

  return m_packet.sender();
}
