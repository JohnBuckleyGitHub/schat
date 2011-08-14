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
#include "User.h"

class PacketReader;

/*!
 * Формирует пакет Protocol::UserDataPacket, содержащий базовую информацию о пользователе.
 *
 * - 01 byte  - options.
 * - 01 byte  - reserved.
 * - 01 byte  - Gender.
 * - 01 byte  - Status.
 * - utf8     - Nickname.
 * - utf8     - UserAgent (is not set Protocol::Broadcast).
 * - utf8     - Host (is not set Protocol::Broadcast).
 */
class SCHAT_EXPORT UserWriter : public PacketWriter
{
public:
  UserWriter(QDataStream *stream, User *user);
  UserWriter(QDataStream *stream, User *user, const QByteArray &destId, const QByteArray &channelId, int options = 0);
  UserWriter(QDataStream *stream, User *user, const QByteArray &destId, int options = 0);

private:
  void write(User *user, int options);
};


/*!
 * Читает пакет Protocol::UserDataPacket.
 */
class SCHAT_EXPORT UserReader
{
public:
  UserReader(PacketReader *reader);

  quint8 options;
  User user;
};

#endif /* USERS_H_ */
