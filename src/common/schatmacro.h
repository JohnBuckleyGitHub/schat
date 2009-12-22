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

#ifndef SCHATMACRO_H_
#define SCHATMACRO_H_

#ifdef SCHAT_DEBUG
  #undef SCHAT_DEBUG
  #define SCHAT_DEBUG(x) qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << x;
  #include <QDebug>
  #include <QTime>
#else
  #define SCHAT_DEBUG(x)
#endif

#define SCHAT_READ_PACKET(x) if (x ::opcodes().contains(opcode)) { \
                               x packet; \
                               if (packet.read(opcode, block)) \
                                 read(&packet); \
                               return; \
                             }

#define SCHAT_EMIT_PACKET(x) if (x ::opcodes().contains(opcode)) { \
                               x packet; \
                               if (packet.read(opcode, block)) \
                                 emitPacket(&packet); \
                               return; \
                             }

#define SCHAT_DETECT_PACKET(x)   if (x ::opcodes().contains(opcode)) { \
                                   x *p = static_cast<x *>(packet); \
                                   read(p); \
                                 }

#endif /* SCHATMACRO_H_ */
