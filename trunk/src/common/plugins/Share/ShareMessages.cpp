/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2014 IMPOMEZIA <schat@impomezia.com>
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
#include "client/ClientMessages.h"
#include "hooks/MessagesImpl.h"
#include "sglobal.h"
#include "ShareMessages.h"
#include "SharePlugin_p.h"

ShareMessages::ShareMessages(Share *share)
  : Messages(share)
  , m_share(share)
{
  ChatClient::messages()->add(this);
}


int ShareMessages::read(MessagePacket packet)
{
  if (packet->command() != LS("image") || Hooks::MessagesImpl::isIgnored(packet))
    return 0;

  if (ChatId(packet->id()).type() != ChatId::MessageId)
    return 0;

  m_share->read(packet);
  return 1;
}
