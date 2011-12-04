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

#ifndef MASTERNODE_H_
#define MASTERNODE_H_

#include "cores/Core.h"

class MasterNode : public Core
{
  Q_OBJECT

public:
  MasterNode(QObject *parent = 0);

protected:
  void accept(const AuthResult &result);
  void notice(quint16 type);
  void reject(const AuthResult &result);
  void release(SocketReleaseEvent *event);

private:
  QHash<QByteArray, QString> m_hosts; ///< Таблица адресов пользователей, используется при авторизации клиентов с вторичного сервера.
};

#endif /* MASTERNODE_H_ */