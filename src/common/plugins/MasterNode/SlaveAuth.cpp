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

#include "MasterNode.h"
#include "net/packets/auth.h"
#include "SlaveAuth.h"
#include "Storage.h"

SlaveAuth::SlaveAuth(MasterNode *node)
  : AnonymousAuth(node)
  , m_node(node)
{
}


AuthResult SlaveAuth::auth(const AuthRequest &data)
{
  qDebug() << "SLAVE AUTH";
  if (Storage::i()->serverData()->privateId() != data.privateId)
    return AuthResult(AuthReplyData::Forbidden);

  AuthResult result = AnonymousAuth::auth(data);
  if (result.action == AuthResult::Accept) {
    Storage *storage = Storage::i();
    QList<QByteArray> slaves = storage->slaves();

    for (int i = 0; i < slaves.size(); ++i) { // Вторичные серверы не могут использовать один номер сервера.
      ChatUser slave = storage->user(slaves.at(i));
      if (slave && slave->rawGender() == data.gender)
        return AuthResult(AuthReplyData::Forbidden);
    }

    Storage::i()->addSlave(result.id);
  }

  return result;
}


int SlaveAuth::type() const
{
  return AuthRequest::SlaveNode;
}
