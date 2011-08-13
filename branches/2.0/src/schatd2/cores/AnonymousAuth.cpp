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

#include <QDebug>

#include "cores/AnonymousAuth.h"
#include "cores/Core.h"
#include "events.h"
#include "net/packets/auth.h"
#include "Storage.h"

AnonymousAuth::AnonymousAuth(Core *core)
  : NodeAuth(core)
{
}


AuthResult AnonymousAuth::auth(const AuthRequestData &data)
{
  Storage *storage = Storage::i();

  QByteArray userId = storage->makeUserId(data.authType, data.uniqueId);
  ChatUser user = storage->user(userId);

  if (user)
    return AuthResult(AuthReplyData::UserIdAlreadyUse);

  QString normalNick = storage->normalize(data.nick);
  if (storage->user(normalNick, false))
    return AuthResult(AuthReplyData::NickAlreadyUse, 0);

  user = ChatUser(new ServerUser(storage->session(), normalNick, userId, data, m_core->packetsEvent()->socket()));
  if (!user->isValid())
    return AuthResult(AuthReplyData::BadUser);

  user->setUserAgent(data.userAgent);
  user->setHost(m_core->packetsEvent()->address.toString());
  storage->add(user); // FIXME не происходит обновление информации о пользователе.

  qDebug() << "ANONYMOUS AUTH" << user->nick() << user->host() << user->userAgent();
  return AuthResult(userId);
}


int AnonymousAuth::type() const
{
  return AuthRequestData::Anonymous;
}
