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

#ifndef ACCOUNTS_H_
#define ACCOUNTS_H_

#include <QVariant>

#include "schat.h"

/*!
 * Ответ на запрос регистрации.
 */
class SCHAT_EXPORT RegReply
{
public:
  /// Результат регистрации.
  enum Result {
    Error = 101, ///< При регистрации произошла ошибка.
    OK = 111     ///< Регистрация успешна.
  };

  /// Тип ошибки.
  enum ErrorType {
    Unknown = 117,            ///< 'u' Неизвестная ошибка.
    BadName = 110,            ///< 'n' Некорректное имя пользователя.
    BadPassword = 112,        ///< 'p' Некорректный пароль.
    ServiceUnavailable = 85,  ///< 'U' Регистрация не доступна.
    InternalError = 105,      ///< 'i' Внутренняя ошибка сервера.
    UserAlreadyExists = 101   ///< 'e' Пользователь уже зарегистрирован.
  };

  RegReply()
  : m_error(Unknown)
  , m_result(Error)
  {}

  RegReply(ErrorType error)
  : m_error(error)
  , m_result(Error)
  {}

  RegReply(const QString &name)
  : m_error(Unknown)
  , m_result(OK)
  , m_name(name)
  {}

  inline int result() const { return m_result; }
  inline QString name() const { return m_name; }
  QVariant json() const;
  static QString filter(const QString &name);

private:
  int m_error;    ///< Тип ошибки.
  int m_result;   ///< Результат регистрации.
  QString m_name; ///< Имя присвоенное при регистрации.
};

#endif /* ACCOUNTS_H_ */
