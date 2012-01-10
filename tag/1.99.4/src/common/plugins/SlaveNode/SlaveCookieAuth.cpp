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
#include "SlaveCookieAuth.h"
#include "SlaveNode.h"

SlaveCookieAuth::SlaveCookieAuth(SlaveNode *node)
  : CookieAuth(node)
  , m_node(node)
{
}

AuthResult SlaveCookieAuth::auth(const AuthRequest &data)
{
  AuthResult result = CookieAuth::auth(data);
  if (result.action == AuthResult::Reject)
    return result;

  if (result.action == AuthResult::Nothing && m_node->mode() == SlaveNode::FailbackMode)
    return AuthResult(Notice::Forbidden, data.id);

  return result;
}


int SlaveCookieAuth::type() const
{
  return AuthRequest::Cookie;
}


BypassSlaveCookieAuth::BypassSlaveCookieAuth(SlaveNode *node)
  : SlaveAnonymousAuth(node)
{
}


int BypassSlaveCookieAuth::type() const
{
  return AuthRequest::Cookie;
}
