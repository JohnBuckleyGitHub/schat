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

#include "Account.h"
#include "Ch.h"
#include "cores/CookieAuth.h"
#include "cores/Core.h"
#include "events.h"
#include "net/packets/auth.h"
#include "NodeLog.h"
#include "Normalize.h"
#include "sglobal.h"
#include "Storage.h"

CookieAuth::CookieAuth(Core *core)
  : AnonymousAuth(core)
{
}


AuthResult CookieAuth::auth(const AuthRequest &data)
{
  if (SimpleID::typeOf(data.cookie) != SimpleID::CookieId)
    return AnonymousAuth::auth(data);

  return auth(data, Ch::channel(data.cookie, SimpleID::UserId));
}


int CookieAuth::type() const
{
  return AuthRequest::Cookie;
}


AuthResult CookieAuth::auth(const AuthRequest &data, ChatChannel channel)
{
  if (!channel)
    return AnonymousAuth::auth(data);

  AuthResult result = isCollision(channel->id(), data.nick, data.id);
  if (result.action == AuthResult::Reject)
    return result;

  if (isPasswordRequired(channel.data(), data.uniqueId)) {
    result = AuthResult(Notice::Unauthorized, data.id);
    result.json[LS("account")] = channel->account()->name();
    return result;
  }

  update(channel.data(), data);
  if (!channel->isValid())
    return AuthResult(Notice::BadRequest, data.id);

  Core::add(channel);
  Ch::newUserChannel(channel, data, m_core->packetsEvent()->address.toString());

  SCHAT_LOG_DEBUG(<< "COOKIE AUTH" << (channel->name() + "@" + m_core->packetsEvent()->address.toString() + "/" + SimpleID::encode(channel->id())) << data.userAgent << data.host);
  return AuthResult(channel->id(), data.id);
}
