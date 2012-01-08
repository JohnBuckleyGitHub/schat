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

#include "Ch.h"
#include "cores/AnonymousAuth.h"
#include "cores/Core.h"
#include "events.h"
#include "net/packets/auth.h"
#include "NodeLog.h"
#include "Normalize.h"
#include "Storage.h"

AnonymousAuth::AnonymousAuth(Core *core)
  : NodeAuth(core)
{
}


/*!
 * \todo Реализовать поддержку различных агентов и адреса пользователя для различных подключений одного из них.
 *
 * \sa StorageHooks::createdNewUserChannel().
 */
AuthResult AnonymousAuth::auth(const AuthRequest &data)
{
  QByteArray id = Storage::i()->makeUserId(data.authType, data.uniqueId);

  if (Ch::isCollision(id, data.nick))
    return AuthResult(Notice::NickAlreadyUse, data.id, 0);

  ChatChannel channel = Ch::channel(id, SimpleID::UserId);
  bool created = false;

  if (!channel) {
    channel = ChatChannel(new ServerChannel(id, data.nick));
    created = true;
  }

  update(channel.data(), data);

  if (!channel->isValid())
    return AuthResult(Notice::BadRequest, data.id);

  m_core->add(channel, data.authType, data.id);
  Ch::newUserChannel(channel, data, m_core->packetsEvent()->address.toString(), created);

  SCHAT_LOG_DEBUG(<< "ANONYMOUS AUTH" << (channel->name() + "@" + m_core->packetsEvent()->address.toString() + "/" + SimpleID::encode(channel->id())) << data.userAgent << data.host);
  return AuthResult(id, data.id);
}


int AnonymousAuth::type() const
{
  return AuthRequest::Anonymous;
}


/*!
 * \todo Добавить обновление информации об адресе и клиенте пользователя.
 */
void AnonymousAuth::update(ServerChannel *channel, const AuthRequest &data)
{
  channel->setName(data.nick);
  channel->gender().setRaw(data.gender);
  channel->status().set(data.status);
  channel->addSocket(m_core->packetsEvent()->socket());
}
