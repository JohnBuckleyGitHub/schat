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

#include "cores/CookieAuth.h"
#include "cores/Core.h"
#include "events.h"
#include "net/packets/auth.h"
#include "NodeLog.h"
#include "Normalize.h"
#include "Storage.h"

CookieAuth::CookieAuth(Core *core)
  : AnonymousAuth(core)
{
}


AuthResult CookieAuth::auth(const AuthRequest &data)
{
  if (SimpleID::typeOf(data.cookie) != SimpleID::CookieId)
    return AnonymousAuth::auth(data);

  return auth(data, Storage::i()->channel(data.cookie, SimpleID::UserId));
}


int CookieAuth::type() const
{
  return AuthRequest::Cookie;
}


AuthResult CookieAuth::auth(const AuthRequest &data, ChatChannel channel)
{
  if (!channel)
    return AnonymousAuth::auth(data);

  ChatChannel exist   = Storage::i()->channel(Normalize::toId('~' + data.nick), SimpleID::UserId);
  if (exist && exist->id() != channel->id())
    return AuthResult(Notice::NickAlreadyUse, data.id, 0);

  update(channel.data(), data);

  if (!channel->isValid())
    return AuthResult(Notice::BadRequest, data.id);

  m_core->add(channel, data.authType, data.id);

  SCHAT_LOG_DEBUG() << "COOKIE AUTH" << (channel->name() + "@" + m_core->packetsEvent()->address.toString() + "/" + SimpleID::encode(channel->id())) << data.userAgent << data.host;
  return AuthResult(channel->id(), data.id);
}
