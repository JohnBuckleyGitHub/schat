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

#ifndef PROTOCOL4_H_
#define PROTOCOL4_H_

/*!
 * \brief Содержит общие определения \b Simple \b Chat протокола.
 */
namespace protocol {

  /// Версия протокола, размер блока 2 байта (quint8 + quint8).
  enum Version {
    MajorVersion = 4, ///< Старшая версия протокола.
    MinorVersion = 0  ///< Младшая версия (модификация) протокола.
  };

  /// Коды пакетов, размер кода 2 байта (quint16).
  enum PacketOpcode {
    Greeting = 7666
  };

  enum Gender {
    Male,
    Female,
    Bot
  };

/*!
 * \brief Определения пакетов \b Simple \b Chat протокола.
 */
namespace packet {

  /*!
   * \brief Абстрактный базовый пакет, размер 4 байта.
   */
  class AbstractPacket {
  public:
    AbstractPacket()
    : size(0), opcode(0) {}

    AbstractPacket(quint16 op)
    : size(0), opcode(op) {}

    quint16 size;   ///< Размер тела пакета, на 4 байта меньше полного размера.
    quint16 opcode; ///< Опкод пакета.
  };
}
}

#endif /* PROTOCOL4_H_ */
