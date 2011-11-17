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

#include <QDebug>

#include "cores/AnonymousAuth.h"
#include "cores/Core.h"
#include "events.h"
#include "net/packets/auth.h"
#include "NodeLog.h"
#include "Storage.h"
#include "Normalize.h"

AnonymousAuth::AnonymousAuth(Core *core)
  : NodeAuth(core)
{
}


/*!
 * \todo Реализовать поддержку различных агентов и адреса пользователя для различных подключений одного из них.
 */
AuthResult AnonymousAuth::auth(const AuthRequest &data)
{
  Storage *storage      = Storage::i();
  QByteArray id         = storage->makeUserId(data.authType, data.uniqueId);
  QByteArray normalized = Normalize::toId('~' + data.nick);
  ChatChannel channel   = storage->channel(normalized, SimpleID::UserId);

  if (channel && channel->id() != id)
    return AuthResult(Notice::NickAlreadyUse, data.id, 0);

  if (!channel)
    channel = storage->channel(id, SimpleID::UserId);

  if (!channel)
    channel = ChatChannel(new ServerChannel(id, data.nick));

  update(channel.data(), data);

  if (!channel->isValid())
    return AuthResult(Notice::BadRequest, data.id);

  m_core->add(channel, data.authType, data.id);

//  SCHAT_LOG_DEBUG() << "ANONYMOUS AUTH" << (user->nick() + "@" + user->host() + "/" + SimpleID::encode(user->id())) << user->userAgent() << data.host;
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


/*!
 * Обновление данных пользователя при авторизации.
 * \deprecated Эта функция устарела.
 *
 * \param user Пользователь.
 * \param data Авторизационные данные.
 */
void AnonymousAuth::update(ServerUser *user, const AuthRequest &data)
{
  user->setNick(data.nick);
  user->gender().setRaw(data.gender);
  user->setStatus(data.status);
  user->addSocket(m_core->packetsEvent()->socket());
  user->setUserAgent(data.userAgent);
  user->setHost(m_core->packetsEvent()->address.toString());
}
