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

#include "Ch.h"
#include "cores/PasswordAuth.h"
#include "DataBase.h"
#include "net/packets/auth.h"
#include "sglobal.h"
#include "Storage.h"

PasswordAuth::PasswordAuth(Core *core)
  : CookieAuth(core)
{
}


AuthResult PasswordAuth::auth(const AuthRequest &data)
{
  if (SimpleID::typeOf(data.password) != SimpleID::PasswordId)
    return AuthResult(Notice::BadRequest, data.id);

  QString name = PasswordAuth::name(data.account);
  if (name.isEmpty())
    return AuthResult(Notice::BadRequest, data.id);

  qint64 key = DataBase::accountKey(name);
  if (key == -1)
    return AuthResult(Notice::NotFound, data.id);

  Account account = DataBase::account(key);
  if (!account.isValid())
    return AuthResult(Notice::Forbidden, data.id);

  if (account.password() != data.password)
    return AuthResult(Notice::Forbidden, data.id);

  ChatChannel channel = Ch::channel(account.cookie(), SimpleID::UserId);
  if (!channel)
    return AuthResult(Notice::InternalError, data.id);

  m_checked = true;
  return CookieAuth::auth(data, channel);
}


int PasswordAuth::type() const
{
  return AuthRequest::Password;
}


QString PasswordAuth::name(const QString &name)
{
  if (name.isEmpty() || name.size() > 255 )
    return QString();

  QString out = name.simplified().toLower().remove(LC(' '));
  int index = out.indexOf(LC('@'));
  if (index != -1)
    out = out.left(index);

  return out;
}
