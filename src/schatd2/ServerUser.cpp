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

#include "net/packets/auth.h"
#include "ServerUser.h"

ServerUser::ServerUser(const QByteArray &session, const QString &normalNick, const QByteArray &id, AuthRequest *authRequest, int workerId, quint64 socketId)
  : User()
  , m_workerId(workerId)
  , m_session(session)
  , m_normalNick(normalNick)
  , m_socketId(socketId)
{
  setId(id);
  setNick(authRequest->nick());
}
