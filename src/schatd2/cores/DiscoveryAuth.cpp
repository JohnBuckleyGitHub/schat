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

#include "cores/DiscoveryAuth.h"
#include "net/packets/auth.h"
#include "Storage.h"

DiscoveryAuth::DiscoveryAuth(Core *core)
  : AnonymousAuth(core)
{
}


AuthResult DiscoveryAuth::auth(const AuthRequest &data)
{
  if (Storage::anonymous() && Storage::authServer().isEmpty())
    return AnonymousAuth::auth(data);

  return AuthResult(Notice::Found, data.id);
}


int DiscoveryAuth::type() const
{
  return AuthRequest::Discovery;
}
