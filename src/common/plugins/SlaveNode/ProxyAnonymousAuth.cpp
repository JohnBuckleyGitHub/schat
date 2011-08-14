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

#include "client/AbstractClient.h"
#include "ProxyAnonymousAuth.h"
#include "SlaveNode.h"

ProxyAnonymousAuth::ProxyAnonymousAuth(SlaveNode *node)
  : AnonymousAuth(node)
  , m_node(node)
{
}


AuthResult ProxyAnonymousAuth::auth(const AuthRequestData &data)
{
  AuthResult result = AnonymousAuth::auth(data);
  if (result.action == AuthResult::Reject || m_node->mode() == SlaveNode::FailbackMode) {
    return result;
  }

  m_node->uplink()->send(m_node->readBuffer()); // FIXME добавить передачу ип адреса.
  result.action = AuthResult::Pending;
  return result;
}
