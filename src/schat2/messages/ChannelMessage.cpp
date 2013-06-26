/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

const QString ChannelMessage::kInternalId = QLatin1String("InternalId");
const QString ChannelMessage::kCommand    = QLatin1String("Command");
const QString ChannelMessage::kDirection  = QLatin1String("Direction");
const QString ChannelMessage::kStatus     = QLatin1String("Status");
const QString ChannelMessage::kOID        = QLatin1String("OID");
const QString ChannelMessage::kMDate      = QLatin1String("MDate");

const QString ChannelMessage::kOutgoing   = QLatin1String("outgoing");
const QString ChannelMessage::kIncoming   = QLatin1String("incoming");
const QString ChannelMessage::kOffline    = QLatin1String("offline");
const QString ChannelMessage::kRejected   = QLatin1String("rejected");

ChannelMessage::ChannelMessage(MessagePacket packet)
  : Message(packet->id(), QByteArray(), LS("channel"), LS("addChannelMessage"))
  , m_packet(packet)
{
  setDate(m_packet->date());
  setAuthor(m_packet->sender());
  m_tab = detectTab(m_packet->sender(), m_packet->dest());

  if (!m_packet->internalId().isEmpty())
    m_data.insert(kInternalId, ChatId(m_packet->internalId()).toString());

  m_data.insert(kCommand,   packet->command());
  m_data.insert(kText,      TokenFilter::filter(LS("channel"), packet->text()));
  m_data.insert(kDirection, m_packet->sender() == ChatClient::id() ? kOutgoing : kIncoming);

  /// Если это собственное сообщение, то для него при необходимости устанавливается
  /// статус "offline" или "rejected".
  const int status = packet->status();
  if (isOffline(status))
    m_data.insert(kStatus, kOffline);
  else if (status != Notice::OK && status != Notice::Found)
    m_data.insert(kStatus, kRejected);

  if (!packet->oid.isNull()) {
    m_data.insert(kOID, QString(ChatId::toBase32(packet->oid.byteArray())));
    m_data.insert(kMDate, packet->mdate);
  }
}


bool ChannelMessage::isOffline(int status)
{
  if (status == Notice::ChannelOffline)
    return true;

  if (status == Notice::Unread)
    return true;

  if (status == Notice::Read)
    return true;

  return false;
}
