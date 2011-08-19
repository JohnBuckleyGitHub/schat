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

#include "ServerChannel.h"

ServerChannel::ServerChannel(ClientChannel channel)
  : Channel(channel->id(), channel->name())
  , m_permanent(false)
  , m_normalName(channel->name())
{
  setDesc(channel->desc());
  setTopic(channel->topic());
  setUsers(channel->users());
}


ServerChannel::ServerChannel(const QByteArray &id, const QString &normalName, const QString &name, bool permanent)
  : Channel(id, name)
  , m_permanent(permanent)
  , m_normalName(normalName)
{
}

ServerChannel::~ServerChannel()
{
}
