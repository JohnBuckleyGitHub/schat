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

#include "net/packets/notices.h"
#include "net/PacketWriter.h"
#include "net/ServerData.h"
#include "net/packets/AbstractPacket.h"

class PacketReader;
class User;

/*!
 * Ответ сервера на запрос авторизации.
 * Заголовок пакета:
 * - \b sender  - Идентификатор сервера.
 * - \b dest    - Идентификатор, выданный сервером клиенту, в случае если статус \p status не Notice::OK это поле отсутствует.
 *
 * Поля данных:
 *  - \b quint8    - Битовая маска AuthReply::Fields дополнительный полей пакета.
 *  - \b quint16   - Статус Notice::StatusCodes.
 *  - \b id        - Уникальный идентификатор авторизации, поле совпадает с таковым из AuthRequest.
 *    - \b id      - Cookie (\p status == Notice::OK).
 *    - \b quint32 - Битовая маска стандартных возможностей сервера ServerData::Features (\p status == Notice::OK).
 *    - \b quint8  - Номер сервера (\p status & AuthReply::JSonField).
 *    - \b utf8    - Имя сервера (\p status & AuthReply::JSonField).
 *  - \b utf8      - JSON данные (\p status & AuthReply::JSonField).
 *    - \b id      - Идентификатор основного канала (\p status == Notice::OK и опции сервера содержат ServerData::AutoJoinSupport).
 */
class SCHAT_EXPORT AuthReply : public AbstractPacket
{
public:
  /// Поля данных.
  enum Fields {
    BasicFields = 0,
    JSonField = 1
  };

  AuthReply()
  : AbstractPacket()
  , status(Notice::InternalError)
  {}

  AuthReply(PacketReader *reader);
  AuthReply(ServerData *data, int status, const QByteArray &id, const QVariant &json = QVariant());
  AuthReply(ServerData *data, User *user, const QByteArray &cookie, const QByteArray &id, const QVariant &json = QVariant());
  QByteArray data(QDataStream *stream) const;

  QByteArray userId;     ///< Идентификатор пользователя, передаётся в заголовке пакета как адрес получателя.
  quint16 status;        ///< Статус \sa Notice::StatusCodes, обязательное поле.
  QByteArray cookie;     ///< Cookie.
  QByteArray id;         ///< Уникальный идентификатор авторизации.
  ServerData serverData; ///< Данные о сервере.
};


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
  : error(0)
  {}

  AuthReplyData(ServerData *data, int error);
  AuthReplyData(ServerData *data, User *user, const QByteArray &cookie);

  QByteArray userId;     ///< Идентификатор пользователя, передаётся в заголовке пакета как адрес получателя.
  quint8 status;         ///< Статус авторизации \sa Status.
  QByteArray cookie;     ///< Cookie.
  quint8 protoVersion;   ///< Максимальная поддерживаемая версия протокола.
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
 * - 21 byte  - Cookie.
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
class SCHAT_EXPORT AuthRequest : public AbstractPacket
{
public:
  /// Тип авторизации.
  enum AuthType {
    Anonymous = 97,   ///< 'a' Анонимная авторизация по уникальному идентификатору.
    SlaveNode = 115,  ///< 's' Авторизация по приватному идентификатору сервера.
    Cookie = 99,      ///< 'c' Сохранённая анонимная авторизация по ранее присвоеному долговременному Cookie.
    Password = 112    ///< 'p' Авторизация по имени пользователя и паролю.
  };

  /// Поля данных.
  enum Fields {
    BasicFields = 0,
    JSonField = 1
  };

  AuthRequest()
  : AbstractPacket()
  , authType(0)
  {}

  AuthRequest(int authType, const QString &host, User *user, const QVariant &json = QVariant());
  AuthRequest(PacketReader *reader);
  QByteArray data(QDataStream *stream) const;
  void setStatus(quint8 status);

  quint8 authType;         ///< Тип авторизации.
  QByteArray uniqueId;     ///< Уникальный идентификатор клиента.
  mutable QByteArray id;   ///< Уникальный идентификатор авторизации.
  quint8 gender;           ///< Пол и цвет пользователя.
  quint8 status;           ///< Статус пользователя.
  QString host;            ///< Адрес по которому клиент подключается к серверу.
  QString nick;            ///< Ник.
  QString userAgent;       ///< Идентификатор клиента пользователя.
  QString privateId;       ///< Приватный идентификатор сервера, только для типа авторизации AuthRequestData::SlaveNode.
  QByteArray cookie;       ///< Cookie, только для типа авторизации AuthRequestData::SlaveNode.
};

#endif /* AUTH_H_ */
