/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include "Account.h"
#include "Ch.h"
#include "cores/NodeAuth.h"
#include "net/NodeAuthReply.h"
#include "sglobal.h"
#include "Storage.h"

NodeAuthReply::NodeAuthReply(const AuthResult &result, ChatChannel channel)
  : AuthReply()
{
  flags      = 0;
  serverId   = Storage::serverId();
  status     = result.status;
  id         = result.authId;
  json       = result.json;
  serverName = Ch::server()->name();

  if (Storage::value(STORAGE_ANONYMOUS_AUTH).toBool())
    flags = 1;

  if (channel) {
    userId   = channel->id();
    cookie   = channel->account()->cookie;
    hostId   = channel->hosts()->id();
    provider = channel->account()->provider;
  }
  else
    provider = Storage::value(STORAGE_AUTH_SERVER).toString();
}
