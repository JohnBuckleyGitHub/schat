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

#include <QString>

#include "net/Packet.h"

/*!
 * - 01 byte  - Status (AccessDenied), \sa Status.
 * - 01 byte  - Error code \sa Error.
 * or
 * - 01 byte  - Status (AccessGranted), \sa Status.
 * - 20 bytes - Server Id (SHA1).
 * - 20 bytes - Client Id (SHA1).
 * - 20 bytes - Session (SHA1).
 */
class AuthReply : public Packet
{
public:
  /// Статус.
  enum Status {
    AccessGranted = 0x11, ///< Доступ разрешён.
    AccessDenied = 0x43   ///< Доступ запрещён.
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

  AuthReply(const QByteArray &serverId, const QByteArray &clientId, const QByteArray &session);
  AuthReply(Error error);
  AuthReply(PacketReader *reader);
  ~AuthReply();
  inline int error() const { return m_error; }
  inline int status() const { return m_status; }
  inline QByteArray serverId() const { return m_serverId; }
  inline QByteArray session() const { return m_session; }
  inline QByteArray userId() const { return m_userId; }
  void body();

private:
  QByteArray m_serverId;
  QByteArray m_session;
  QByteArray m_userId;
  quint8 m_error;
  quint8 m_status;
};


/*!
 * Пакет запроса авторизации.
 *
 * - 01 byte  - AuthVersion.
 * - 01 byte  - AuthType.
 * - 21 byte  - Unique User Id.
 * - 01 byte  - Max Supported Protocol Version.
 * - 04 bytes - Client Features.
 * - 01 byte  - Language.
 * - not fixed length (utf8) - Server Host.
 * - not fixed length (utf8) - Nickname.
 * - not fixed length (utf8) - User Agent.
 */
class AuthRequest : public Packet
{
public:
  /// Версия пакета.
  enum AuthVersion {
    V1 = 0x1
  };

  /// Тип авторизации.
  enum AuthType {
    Anonymous = 0x2A, ///< Анонимная авторизация по уникальному идентификатору.
    Partially,        ///< Восстановление разорванного соединения, для авторизации используется сессия.
    Cookie,           ///< Сохранённая анонимная авторизация по ранее присвоеному долговременному Cookie.
    Password          ///< Авторизация по имени пользователя и паролю.
  };

  /// Возможности клиента.
  enum ClientFeatures {
    SupportRichText = 0x1 ///< Поддержка HTML текста.
  };

  AuthRequest(PacketReader *reader);
  AuthRequest(AuthType authType, const QString &host, const QString &nick);
  ~AuthRequest();
  bool isValid() const;
  inline int authType() const { return m_authType; }
  inline int authVersion() const { return m_authVersion; }
  inline int features() const { return m_features; }
  inline int language() const { return m_language; }
  inline QByteArray uniqueId() const { return m_uniqueId; }
  inline QString host() const { return m_host; }
  inline QString nick() const { return m_nick; }
  inline QString userAgent() const { return m_userAgent; }
  static QByteArray genUniqueId();
  static QString genUserAgent();
  void body();

private:
  PacketReader *m_reader;    ///< Объект читающий пакет.
  QByteArray m_uniqueId;     ///< Уникальный идентификатор клиента.
  QString m_host;            ///< Адрес по которому клиент подключается к серверу.
  QString m_nick;            ///< Ник.
  QString m_userAgent;       ///< Идентификатор клиента пользователя.
  quint32 m_features;        ///< Возможности клиента.
  quint8 m_authType;         ///< Тип авторизации.
  quint8 m_authVersion;      ///< Версия пакета.
  quint8 m_language;         ///< Язык клиента.
  quint8 m_maxProtoVersion;  ///< Max Supported Protocol Version.
};

#endif /* AUTH_H_ */
