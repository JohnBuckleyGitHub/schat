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
 * - quint8   - Fields.
 * - quint8   - reserved.
 * - quint8   - Gender.
 * - quint8   - Status.
 * - utf8     - Nickname.
 * - utf8     - UserAgent (если установлен флаг StaticData).
 * - utf8     - Host (если установлен флаг StaticData).
 * - utf8     - Группы пользователя (если установлен флаг StaticData).
 * - utf8     - Аккаунт пользователя (если установлен флаг StaticData).
 * - utf8     - JSON данные (если установлен флаг JSonData).
 */
class SCHAT_EXPORT UserWriter : public PacketWriter
{
public:
  /// Опции данных.
  enum Fields {
    NoOptions = 0,
    StaticData = 1, ///< Пакет содержит не изменяемую информацию о пользователе, например UserAgent, адрес и т.д.
    AuthData = 2,   ///< Пакет содержит авторизационную информацию, используется только для синхронизации данных пользователей между серверами.
    JSonData = 4    ///< Пакет содержит JSON данные содержащую дополнительную информацию о пользователе.
  };

  UserWriter(QDataStream *stream, User *user);
  UserWriter(QDataStream *stream, User *user, const QByteArray &destId, const QByteArray &channelId, int options);
  UserWriter(QDataStream *stream, User *user, const QByteArray &destId, int options);
  UserWriter(QDataStream *stream, User *user, const QList<QByteArray> dest, const QByteArray &cookie);

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

  quint8 fields;
  User user;
  QByteArray cookie;
};

#endif /* USERS_H_ */
