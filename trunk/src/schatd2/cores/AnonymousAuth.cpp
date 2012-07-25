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

#include "Account.h"
#include "Ch.h"
#include "cores/AnonymousAuth.h"
#include "cores/Core.h"
#include "events.h"
#include "net/packets/auth.h"
#include "NodeLog.h"
#include "Normalize.h"
#include "sglobal.h"
#include "Storage.h"

QHash<QByteArray, quint64> AnonymousAuth::m_collisions;

AnonymousAuth::AnonymousAuth(Core *core)
  : NodeAuth(core)
{
}


AuthResult AnonymousAuth::auth(const AuthRequest &data)
{
  QByteArray id = Ch::userId(data.uniqueId);

  AuthResult result = isCollision(id, data.nick, data.id);
  if (result.action == AuthResult::Reject)
    return result;

  ChatChannel channel = Ch::channel(id, SimpleID::UserId);
  bool created = false;

  if (!channel) {
    channel = ChatChannel(new ServerChannel(id, data.nick));
    created = true;

    channel->setAccount();
    channel->account()->groups += LS("anonymous");
    channel->setName(data.nick);
    channel->gender().setRaw(data.gender);
  }

  update(channel.data(), data);
  if (!channel->isValid())
    return AuthResult(Notice::BadRequest, data.id);

  Core::add(channel);
  Ch::newUserChannel(channel, data, m_core->packetsEvent()->address.toString(), created);

  SCHAT_LOG_INFO_STR("ANONYMOUS AUTH: " + channel->name().toUtf8() + '@' + m_core->packetsEvent()->address.toString().toUtf8() + '/' + SimpleID::encode(channel->id()) + ", " + data.host.toUtf8());
  return AuthResult(id, data.id);
}


int AnonymousAuth::type() const
{
  return AuthRequest::Anonymous;
}


/*!
 * Проверка на коллизию ника.
 * Допускается не более 20 попыток разрешить коллизию без разрыва соединения.
 *
 * \param id     Идентификатор канала.
 * \param name   Имя канала.
 * \param authId Идентификатор авторизации.
 */
AuthResult AnonymousAuth::isCollision(const QByteArray &id, const QString &name, const QByteArray &authId, bool override)
{
  quint64 collisions = m_collisions.value(id);

  if (Ch::isCollision(id, name, override)) {
    ++collisions;
    m_collisions[id] = collisions;

    if (collisions == 10)
      return AuthResult(Notice::NickAlreadyUse, authId);

    return AuthResult(Notice::NickAlreadyUse, authId, collisions > 20 ? 2 : 0);
  }

  m_collisions.remove(id);
  return AuthResult();
}


void AnonymousAuth::update(ServerChannel *channel, const AuthRequest &data)
{
  if (channel->status().value() == Status::Offline)
    channel->status().set(data.status);
}
