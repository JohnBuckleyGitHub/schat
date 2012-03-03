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

#ifndef HOSTS_H_
#define HOSTS_H_

#include <QMap>

#include "schat.h"

class AuthRequest;

class SCHAT_EXPORT Hosts
{
public:
  Hosts();
  inline QList<quint64> sockets() const { return m_sockets.keys(); }
  void add(quint64 socket, const AuthRequest &data);
  void remove(quint64 socket);

private:
  QMap<quint64, QByteArray> m_sockets; ///< Таблица сокетов и уникальных идентификаторов пользователя.
};

#endif /* HOSTS_H_ */
