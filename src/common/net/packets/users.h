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

#include "net/Packet.h"

class User;

/*!
 * - 01 byte  - options.
 * - 01 byte  - reserved.
 * - not fixed length (utf8) - Nickname.
 */
class UserData : public Packet
{
public:
  UserData(PacketReader *reader);
  UserData(User *user);
  UserData(User *user, const QByteArray &channelId);
  bool isValid() const;
  inline int options() const { return m_options; }
  User *user() { return m_user; }
  void body();

private:
  quint8 m_options;
  User *m_user;
};

#endif /* USERS_H_ */
