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

#include "debugstream.h"
#include "net/packets/auth.h"
#include "ServerUser.h"

ServerUser::ServerUser(const QByteArray &id)
  : User()
  , m_online(false)
  , m_key(-1)
  , m_socketId(0)
{
  SCHAT_DEBUG_STREAM("USER CREATED" << this);
  setId(id);
}


ServerUser::ServerUser(const QByteArray &session, const QString &normalNick, const QByteArray &id, const AuthRequestData &authRequestData, quint64 socketId)
  : User()
  , m_online(true)
  , m_session(session)
  , m_key(-1)
  , m_normalNick(normalNick)
  , m_socketId(socketId)
{
  setId(id);
  setNick(authRequestData.nick);
  setRawGender(authRequestData.gender);
  setStatus(authRequestData.status);
}


ServerUser::~ServerUser()
{
  SCHAT_DEBUG_STREAM("~" << this)
}
