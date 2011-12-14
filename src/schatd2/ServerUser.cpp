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
{
  SCHAT_DEBUG_STREAM("USER CREATED" << this);
  setId(id);
}


ServerUser::ServerUser(const QString &normalNick, const QByteArray &id, const AuthRequest &data, quint64 socket)
  : User()
  , m_online(true)
  , m_uniqueId(data.uniqueId)
  , m_key(-1)
  , m_normalNick(normalNick)
{
  setId(id);
  setNick(data.nick);
  m_gender.setRaw(data.gender);
  setStatus(data.status);
  addSocket(socket);
}


ServerUser::ServerUser(const User *user)
  : User(user)
{
}


ServerUser::~ServerUser()
{
  SCHAT_DEBUG_STREAM("~" << this)
}
