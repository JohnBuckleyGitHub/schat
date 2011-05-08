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

#ifndef USERS_H_
#define USERS_H_

#include "net/PacketWriter.h"

class PacketReader;
class User;

/*!
 * Формирует пакет Protocol::UserDataPacket.
 *
 * - 01 byte  - options.
 * - 01 byte  - reserved.
 * - not fixed length (utf8) - Nickname.
 */
class UserWriter : public PacketWriter
{
public:
  UserWriter(QDataStream *stream, User *user, const QByteArray &destId = QByteArray(), int options = 0);
};


/*!
 * Читает пакет Protocol::UserDataPacket.
 */
class UserReader
{
public:
  UserReader(PacketReader *reader);

  quint8 options;
  User *user;
};

#endif /* USERS_H_ */
