/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#ifndef PACKET_H_
#define PACKET_H_

#include <QByteArray>

struct UserData;

class Packet {
  Packet() {}

public:
  static QByteArray create(quint16 opcode);
  static QByteArray create(quint16 opcode, quint16 data1);
  static QByteArray create(quint16 opcode, quint8 echo, quint8 numeric, const UserData &data);
};


#endif /* PACKET_H_ */
