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

#ifndef AUTH_H_
#define AUTH_H_

#include "net/PacketWriter.h"

class PacketReader;
class User;

/*!
 * Данные ответа сервера на запрос авторизации.
 */
class AuthReplyData
{
public:
  /// Статус.
  enum Status {
    AccessGranted = 71, ///< 'G' Доступ разрешён.
    AccessDenied = 68   ///< 'D' Доступ запрещён.
  };

  /// Коды ошибок.
  enum Error {
    NoError = 0,
    AuthTypeNotImplemented = 40, ///< Выбранный тип авторизации не реализован.
    AuthTypeNotAllowed = 41,     ///< Выбранный тип авторизации запрещён на сервере.
    UserIdAlreadyUse = 42,       ///< Идентификатор пользователя уже используется.
    NickAlreadyUse = 43,         ///< Ник уже используется.
    InvalidAuthRequest = 44,     ///< Некорректный пакет AuthRequest.
    InvalidUser = 45,            ///< Некорректные данные пользователя.
    InternalServerError = 46     ///< Внутренняя ошибка сервера.
  };

  AuthReplyData()
  : error(0)
  , status(0)
  {}

  AuthReplyData(const QByteArray &serverId, const QByteArray &userId, const QByteArray &session);
  AuthReplyData(const QByteArray &serverId, int error);

  QByteArray serverId; ///< Идентификатор сервера.
  QByteArray session;  ///< Сессия.
  QByteArray userId;   ///< Идентификатор пользователя.
  quint8 error;        ///< Код ошибки \sa Error.
  quint8 status;       ///< Статус авторизации \sa Status.
};


/*!
 * Формирует пакет Protocol::AuthReplyPacket.
 *
 * - sender:  - Идентификатор сервера.
 * - dest:    - Идентификатор, выданный сервером клиенту, в случае если статус AccessDenied это поле отсутствует.
 * - 01 byte  - Статус аутентификации(AccessGranted или AccessDenied), \sa Status.
 *
 * Дальнейшие данные различны в зависимости от статуса.
 * Если статус AccessGranted:
 * - 21 byte  - Сессия.
 *
 * Если статус AccessDenied.
 * - 01 byte  - Error code \sa Error.
 */
class AuthReplyWriter : public PacketWriter
{
public:
  AuthReplyWriter(QDataStream *stream, const AuthReplyData &data);
};


/*!
 * Читает пакет Protocol::AuthReplyPacket.
 */
class AuthReplyReader
{
public:
  AuthReplyReader(PacketReader *reader);

  AuthReplyData data;
};


/*!
 * Данные запроса авторизации.
 */
class AuthRequestData
{
public:
  /// Версия пакета.
  enum AuthVersion {
    V1 = 0x1
  };

  /// Тип авторизации.
  enum AuthType {
    Anonymous = 97,  ///< 'a' Анонимная авторизация по уникальному идентификатору.
    Partially = 112, ///< 'p' Восстановление разорванного соединения, для авторизации используется сессия.
    Cookie = 99,     ///< 'c' Сохранённая анонимная авторизация по ранее присвоеному долговременному Cookie.
    Password = 80    ///< 'P' Авторизация по имени пользователя и паролю.
  };

  /// Возможности клиента.
  enum ClientFeatures {
    SupportRichText = 0x1 ///< Поддержка HTML текста.
  };

  AuthRequestData()
  : features(0)
  , authType(0)
  , authVersion(0)
  , maxProtoVersion(0)
  {}

  AuthRequestData(int authType, const QString &host, User *user);
  static QString genUserAgent();

  QByteArray uniqueId;     ///< Уникальный идентификатор клиента.
  QString host;            ///< Адрес по которому клиент подключается к серверу.
  QString nick;            ///< Ник.
  QString userAgent;       ///< Идентификатор клиента пользователя.
  quint32 features;        ///< Возможности клиента.
  quint8 authType;         ///< Тип авторизации.
  quint8 authVersion;      ///< Версия пакета.
  quint8 gender;           ///< Пол и цвет пользователя.
  quint8 language;         ///< Язык клиента.
  quint8 maxProtoVersion;  ///< Max Supported Protocol Version.
};


/*!
 * Формирует пакет Protocol::AuthRequestPacket.
 *
 * - 01 byte  - AuthVersion.
 * - 01 byte  - AuthType.
 * - 21 byte  - Unique User Id.
 * - 01 byte  - Max Supported Protocol Version.
 * - 04 bytes - Client Features.
 * - 01 byte  - Language.
 * - 01 byte  - Gender.
 * - not fixed length (utf8) - Server Host.
 * - not fixed length (utf8) - Nickname.
 * - not fixed length (utf8) - User Agent.
 */
class AuthRequestWriter : public PacketWriter
{
public:
  AuthRequestWriter(QDataStream *stream, const AuthRequestData &data);
};


/*!
 * Читает пакет Protocol::AuthRequestPacket.
 */
class AuthRequestReader
{
public:
  AuthRequestReader(PacketReader *reader);

  AuthRequestData data;
};

#endif /* AUTH_H_ */
