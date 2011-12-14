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

#include "cores/PasswordAuth.h"
#include "DataBase.h"
#include "net/packets/auth.h"
#include "Storage.h"

PasswordAuth::PasswordAuth(Core *core)
  : CookieAuth(core)
{
}


AuthResult PasswordAuth::auth(const AuthRequest &data)
{
  Storage *storage = Storage::i();
  if (!storage->serverData()->is(ServerData::PasswordAuthSupport))
    return CookieAuth::auth(data);

  if (SimpleID::typeOf(data.password) != SimpleID::PasswordId)
    return AuthResult(Notice::BadRequest, data.id);

  QString login = LoginReply::filter(data.account, storage->serverData()->name());
  if (login.isEmpty())
    return AuthResult(Notice::BadRequest, data.id);

//  Account account = storage->db()->account(login);
//  if (!account.isValid())
//    return AuthResult(Notice::Forbidden, data.id);
//
//  if (account.password != data.password)
//    return AuthResult(Notice::Forbidden, data.id);
//
//  ChatUser user = storage->user(account.userId, true);
//  if (!user)
//    return AuthResult(Notice::InternalError, data.id);

//  return CookieAuth::auth(data, user);
  return CookieAuth::auth(data);
}


int PasswordAuth::type() const
{
  return AuthRequest::Password;
}
