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

#include "ChatCore.h"
#include "client/ChatClient.h"
#include "net/SimpleID.h"
#include "RawFeedsMessage.h"

RawFeedsMessage::RawFeedsMessage(const QByteArray &tab, const QByteArray &json)
  : Message()
{
  m_tab = tab;
  m_func = "addRawFeedsMessage";

  m_data["Type"] = "raw-feeds";
  m_data["Id"]   = SimpleID::encode(ChatCore::randomId());
  m_data["Text"] = json;

  m_data["Date"] = ChatClient::date();
}
