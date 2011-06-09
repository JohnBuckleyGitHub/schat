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

#include <QSqlQuery>
#include <QVariant>

#include "debugstream.h"
#include "net/packets/auth.h"
#include "ServerUser.h"

ServerUser::ServerUser(const QByteArray &session, const QString &normalNick, const QByteArray &id, AuthRequestData *authRequestData, int workerId, quint64 socketId)
  : User()
  , m_online(true)
  , m_workerId(workerId)
  , m_session(session)
  , m_key(-1)
  , m_normalNick(normalNick)
  , m_socketId(socketId)
{
  setId(id);
  setNick(authRequestData->nick);
  setRawGender(authRequestData->gender);
  addUser(m_id);
}


ServerUser::ServerUser(const QSqlQuery &query)
  : User()
  , m_online(false)
  , m_workerId(-1)
  , m_key(query.value(IdColumn).toLongLong())
  , m_normalNick(query.value(NormalNickColumn).toString())
  , m_socketId(0)
{
  SCHAT_DEBUG_STREAM("new" << this);

  setId(query.value(UserIdColumn).toByteArray());
  setNick(query.value(NickColumn).toString());
  setRawGender(query.value(GenderColumn).toInt());
  setHost(query.value(HostColumn).toString());
  setUserAgent(query.value(UserAgentColumn).toString());
  addUser(m_id);
}


ServerUser::~ServerUser()
{
  SCHAT_DEBUG_STREAM("~" << this)
}


bool ServerUser::addId(int type, const QByteArray &id)
{
  if (User::addId(type, id)) {
    addUser(id);
    return true;
  }

  return false;
}


bool ServerUser::addUser(const QByteArray &id)
{
  if (m_users.contains(id) || SimpleID::typeOf(id) != SimpleID::UserId)
    return false;

  m_users.append(id);
  return true;
}


bool ServerUser::removeUser(const QByteArray &id)
{
  if (!m_users.contains(id))
    return false;

  QHashIterator<int, QList<QByteArray> > i(m_ids);
  while (i.hasNext()) {
    i.next();
    if (i.value().contains(id))
      return false;
  }

  m_users.removeAll(id);
  return true;
}


void ServerUser::addUsers(const QList<QByteArray> &users)
{
  for (int i = 0; i < users.size(); ++i) {
    addUser(users.at(i));
  }
}


void ServerUser::removeUsers(const QList<QByteArray> &users)
{
  for (int i = 0; i < users.size(); ++i) {
    removeUser(users.at(i));
  }
}
