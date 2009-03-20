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

#include <QtCore>

#include "packet.h"
#include "protocol.h"

/*!
 * Пакеты:
 * - OpcodePing
 * - OpcodePong
 */
QByteArray Packet::create(quint16 opcode)
{
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(StreamVersion);
  out << quint16(0) << opcode;
  out.device()->seek(0);
  out << quint16(block.size() - (int) sizeof(quint16));
  return block;
}


/*!
 * Пакеты:
 * - OpcodeAccessGranted
 */
QByteArray Packet::create(quint16 opcode, quint16 data1)
{
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(StreamVersion);
  out << quint16(0) << opcode << data1;
  out.device()->seek(0);
  out << quint16(block.size() - (int) sizeof(quint16));
  return block;
}
