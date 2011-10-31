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
#include "Storage.h"

CookieAuth::CookieAuth(Core *core)
  : AnonymousAuth(core)
{
}


AuthResult CookieAuth::auth(const AuthRequest &data)
{
  if (SimpleID::typeOf(data.cookie) != SimpleID::CookieId)
    return AuthResult();

  Storage *storage = Storage::i();
  ChatUser exist = storage->user(data.cookie, true);
  if (!exist)
    return AuthResult();

  QString normalNick = storage->normalize(data.nick);
  ChatUser user      = storage->user(normalNick, false);

  // Если пользователь с указанным ником подключен к серверу
  // и его идентификатор не равен идентификатору пользователя, отклоняем авторизацию.
  if (user && user->id() != exist->id())
    return AuthResult(Notice::NickAlreadyUse, data.id, 0);

  user = exist;
  update(user.data(), data);
  m_core->add(user, data.authType, data.id);

  SCHAT_LOG_DEBUG() << "COOKIE AUTH" << user->nick() << user->host() << SimpleID::encode(user->id()) << user->userAgent() << data.host;
  return AuthResult(user->id(), data.id);
}


int CookieAuth::type() const
{
  return AuthRequest::Cookie;
}


BypassCookieAuth::BypassCookieAuth(Core *core)
  : AnonymousAuth(core)
{
}


int BypassCookieAuth::type() const
{
  return AuthRequest::Cookie;
}
