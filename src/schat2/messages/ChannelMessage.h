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

#ifndef CHANNELMESSAGE_H_
#define CHANNELMESSAGE_H_

#include "schat.h"

class MessagePacket;

class SCHAT_CORE_EXPORT ChannelMessage
{
public:
  ChannelMessage(const MessagePacket &packet);
  inline const MessagePacket &packet() const { return m_packet; }

private:
  const MessagePacket &m_packet; ///< Пакет на основе которого формируется сообщение.
};

#endif /* CHANNELMESSAGE_H_ */
