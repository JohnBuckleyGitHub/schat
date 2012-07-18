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

#include "AuthState.h"
#include "net/SimpleID.h"

AuthStateData::AuthStateData(const QByteArray &state, const QByteArray &error)
  : error(error)
  , state(state)
{
}


AuthStateData::AuthStateData(const QByteArray &state, const QByteArray &provider, const QByteArray &id, const QVariantMap &raw, const QByteArray &cookie)
  : cookie(cookie)
  , id(id)
  , provider(provider)
  , state(state)
  , raw(raw)
{
  if (cookie.isEmpty())
    this->cookie = SimpleID::encode(SimpleID::randomId(SimpleID::CookieId));
}


AuthState::AuthState(QObject *parent)
  : QObject(parent)
{
}


void AuthState::add(AuthStateData *data)
{
  AuthStatePtr ptr = AuthStatePtr(data);
  const QByteArray &state = ptr->state;

  m_states[state] = ptr;
  emit added(state, ptr);
}
