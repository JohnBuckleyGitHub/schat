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

#include "client/ChatClient.h"
#include "messages/ChannelMessage.h"
#include "net/packets/MessageNotice.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "text/TokenFilter.h"

ChannelMessage::ChannelMessage(MessagePacket packet)
  : Message()
  , m_packet(packet)
{
  m_data[LS("Type")]      = LS("channel");
  m_data[LS("Id")]        = SimpleID::encode(packet->id());
  m_data[LS("Command")]   = packet->command();
  m_data[LS("Text")]      = TokenFilter::filter(LS("channel"), packet->text());
  m_data[LS("Direction")] = m_packet->sender() == ChatClient::id() ? LS("outgoing") : LS("incoming");
  m_data[LS("Date")]      = m_packet->date();

  /// Если это собственное сообщение, то для него при необходимости устанавливается
  /// статус "offline" или "rejected".
  int status = packet->status();
  if (isOffline(status))
    m_data[LS("Status")] = LS("offline");
  else if (status != Notice::OK && status != Notice::Found)
    m_data[LS("Status")] = LS("rejected");

  if (isFullDate(status))
    m_data["Day"] = true;

  author(m_packet->sender());

  m_tab = detectTab();
}


bool ChannelMessage::isFullDate(int status)
{
  if (status == Notice::Found)
    return true;

  if (status == Notice::Undelivered)
    return true;

  return false;
}


bool ChannelMessage::isOffline(int status)
{
  if (status == Notice::ChannelOffline)
    return true;

  if (status == Notice::Undelivered)
    return true;

  return false;
}


QByteArray ChannelMessage::detectTab() const
{
  if (SimpleID::typeOf(m_packet->dest()) == SimpleID::ChannelId)
    return m_packet->dest();

  if (m_packet->sender() == ChatClient::id())
    return m_packet->dest();

  return m_packet->sender();
}
