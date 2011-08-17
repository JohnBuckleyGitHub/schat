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

#include "events.h"
#include "MasterAnonymousAuth.h"
#include "MasterNode.h"
#include "Storage.h"

MasterAnonymousAuth::MasterAnonymousAuth(MasterNode *node)
  : AnonymousAuth(node)
  , m_node(node)
{
}

AuthResult MasterAnonymousAuth::auth(const AuthRequestData &data)
{
  if (!Storage::i()->isSlave(m_core->packetsEvent()->userId()))
    return AnonymousAuth::auth(data);

  AuthResult result = AnonymousAuth::auth(data);
  result.option = 0;

  if (result.action == AuthResult::Accept) {
    ChatUser user = Storage::i()->user(m_core->packetsEvent()->userId());
    if (user)
      user->addUser(result.id);
  }

  return result;
}
