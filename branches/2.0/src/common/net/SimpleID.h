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

#ifndef SIMPLEID_H_
#define SIMPLEID_H_

#include <QByteArray>

class SimpleID
{
public:
  /// Значения по умолчанию.
  enum Defaults {
    DefaultSize = 21 ///< Размер идентификатора в байтах.
  };

  /// Типы идентификаторов.
  enum IdTypes {
    InvalidId = 0x49,        ///< 'I' Неверный идентификатор.
    UniqueUserId = 0x55,     ///< 'U' Уникальный идентификатор пользователя, использующийся при авторизации.
    UserId = 0x75,           ///< 'u' Стандартный идентификатор пользователя.
    ServerId = 0x53,         ///< 'S' Идентификатор сервера.
    SessionId = 0x73,        ///< 's' Идентификатор сессии.
    ChannelId = 0x63,        ///< 'c' Идентификатор канала.
    PrivateChannelId = 0x50  ///< 'P' Идентификатор приватного канала.
  };

  SimpleID() {}
  static int typeOf(const QByteArray &id);
  static QByteArray id(const QByteArray &userId1, const QByteArray &userId2);
  static QByteArray session(const QByteArray &id);
  static QByteArray uniqueId();
};

#endif /* SIMPLEID_H_ */
