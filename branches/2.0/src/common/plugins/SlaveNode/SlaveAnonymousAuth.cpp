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
#include "SlaveAnonymousAuth.h"
#include "SlaveNode.h"

SlaveAnonymousAuth::SlaveAnonymousAuth(SlaveNode *node)
  : AnonymousAuth(node)
  , m_node(node)
{
}


AuthResult SlaveAnonymousAuth::auth(const AuthRequest &data)
{
  AuthResult result = AnonymousAuth::auth(data);
  if (result.action == AuthResult::Reject)
    return result;

  if (m_node->mode() == SlaveNode::FailbackMode)
    return AuthResult(Notice::Forbidden, data.id);

  result.action = AuthResult::Pending;
  return result;
}
