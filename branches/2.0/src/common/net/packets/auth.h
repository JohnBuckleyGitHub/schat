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
 *    - \b utf8    - Имя аккаунта пользователя если пользователь зарегистрирован или пустая строка (\p status & AuthReply::JSonField).
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
  QString account;       ///< Имя аккаунта пользователя.
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
