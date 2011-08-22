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
#include "net/ServerData.h"

class PacketReader;
class User;

/*!
 * Данные ответа сервера на запрос авторизации.
 */
class SCHAT_EXPORT AuthReplyData
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
    Forbidden = 40,               ///< Доступ запрещён.
    AuthTypeNotAllowed = 41,      ///< Выбранный тип авторизации запрещён на сервере.
    UserIdAlreadyUse = 42,        ///< Идентификатор пользователя уже используется.
    NickAlreadyUse = 43,          ///< Ник уже используется.
    BadAuthRequest = 44,          ///< Некорректный пакет AuthRequest.
    BadUser = 45,                 ///< Некорректные данные пользователя.
    InternalServerError = 100,    ///< Внутренняя ошибка сервера.
    AuthTypeNotImplemented = 101, ///< Выбранный тип авторизации не реализован.
  };

  AuthReplyData()
  : status(0)
  , error(0)
  {}

  AuthReplyData(ServerData *data, int error);
  AuthReplyData(ServerData *data, User *user);

  QByteArray userId;     ///< Идентификатор пользователя, передаётся в заголовке пакета как адрес получателя.
  quint8 status;         ///< Статус авторизации \sa Status.
  QByteArray session;    ///< Сессия.
  quint8 protoVersion;   ///< Максимальная поддерживаемая версия протокола.
  QString host;          ///< Адрес пользователя.
  quint8 error;          ///< Код ошибки \sa Error.
  ServerData serverData; ///< Данные о сервере.
};


/*!
 * Формирует пакет Protocol::AuthReplyPacket.
 *
 * - sender:  - Идентификатор сервера.
 * - dest:    - Идентификатор, выданный сервером клиенту, в случае если статус AccessDenied это поле отсутствует.
 * - 01 byte  - Статус аутентификации(AccessGranted или AccessDenied), \sa Status.
 * - 01 byte  - номер сервера.
 *
 * Дальнейшие данные различны в зависимости от статуса.
 * Если статус AccessGranted:
 * - 21 byte  - Сессия.
 * - 01 byte  - Версия протокола.
 * - 02 bytes - Разрешающие разрешения.
 * - 02 bytes - Запрещающие разрешения.
 * - 04 bytes - Возможности сервера.
 * - 21 byte  - Идентификатор основного канала (если установлена опция ServerData::AutoJoinSupport)
 * - utf8     - Имя сервера.
 * - utf8     - Адрес пользователя.
 *
 * Если статус AccessDenied.
 * - 01 byte  - Error code \sa Error.
 */
class SCHAT_EXPORT AuthReplyWriter : public PacketWriter
{
public:
  AuthReplyWriter(QDataStream *stream, const AuthReplyData &data);
};


/*!
 * Читает пакет Protocol::AuthReplyPacket.
 */
class SCHAT_EXPORT AuthReplyReader
{
public:
  AuthReplyReader(PacketReader *reader);

  AuthReplyData data;
};


/*!
 * Данные запроса авторизации.
 */
class SCHAT_EXPORT AuthRequestData
{
public:
  /// Версия пакета.
  enum AuthVersion {
    V1 = 0x1
  };

  /// Тип авторизации.
  enum AuthType {
    Anonymous = 0x61,  ///< 'a' Анонимная авторизация по уникальному идентификатору.
    SlaveNode = 0x73,  ///< 's' Авторизация по приватному идентификатору сервера.
    Partially = 0x70,  ///< 'p' Восстановление разорванного соединения, для авторизации используется сессия.
    Cookie = 0x63,     ///< 'c' Сохранённая анонимная авторизация по ранее присвоеному долговременному Cookie.
    Password = 0x50    ///< 'P' Авторизация по имени пользователя и паролю.
  };

  /// Возможности клиента.
  enum ClientFeatures {
    NoFeatures,
    SupportRichText ///< Поддержка HTML текста.
  };

  AuthRequestData()
  : authVersion(0)
  , authType(0)
  , maxProtoVersion(0)
  , features(0)
  {}

  AuthRequestData(int authType, const QString &host, User *user);
  void setStatus(quint8 status);

  quint8 authVersion;      ///< Версия пакета.
  quint8 authType;         ///< Тип авторизации.
  QByteArray uniqueId;     ///< Уникальный идентификатор клиента.
  quint8 maxProtoVersion;  ///< Максимальная поддерживаемая версия протокола.
  quint32 features;        ///< Возможности клиента.
  quint8 language;         ///< Язык клиента.
  quint8 gender;           ///< Пол и цвет пользователя.
  quint8 status;           ///< Статус пользователя.
  QString host;            ///< Адрес по которому клиент подключается к серверу.
  QString nick;            ///< Ник.
  QString userAgent;       ///< Идентификатор клиента пользователя.
  QString privateId;       ///< Приватный идентификатор сервера, только для типа авторизации AuthRequestData::SlaveNode.
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
 * - 01 byte  - User Status.
 * - utf8     - Server Host.
 * - utf8     - Nickname.
 * - utf8     - User Agent.
 */
class SCHAT_EXPORT AuthRequestWriter : public PacketWriter
{
public:
  AuthRequestWriter(QDataStream *stream, const AuthRequestData &data);
};


/*!
 * Читает пакет Protocol::AuthRequestPacket.
 */
class SCHAT_EXPORT AuthRequestReader
{
public:
  AuthRequestReader(PacketReader *reader);

  AuthRequestData data;
};

#endif /* AUTH_H_ */
