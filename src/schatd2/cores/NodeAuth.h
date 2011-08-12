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

#ifndef NODEAUTH_H_
#define NODEAUTH_H_

#include <QByteArray>

class AuthRequestData;
class Core;

class AuthResult
{
public:
  enum Action {
    Reject,
    Accept,
    Nothing
  };

  AuthResult()
  : action(Nothing)
  , error(0)
  , option(0)
  {}

  AuthResult(int error, int option = 2)
  : action(Reject)
  , error(error)
  , option(option)
  {}

  AuthResult(const QByteArray &id)
  : action(Accept)
  , error(0)
  , option(1)
  , id(id)
  {}

  virtual ~AuthResult() {}

  Action action; ///< Действие.
  int error;     ///< Код ошибки.
  int option;    ///< Дополнительная опция для установки состояния сокета.
  QByteArray id; ///< Идентификатор пользователя.
};


/*!
 * Базовый класс модуля авторизации.
 */
class NodeAuth
{
public:
  NodeAuth(Core *core)
  : m_core(core)
  {}

  virtual ~NodeAuth() {}
  virtual AuthResult auth(const AuthRequestData &data) = 0;
  virtual int type() const = 0;

protected:
  Core *m_core;
};

#endif /* NODEAUTH_H_ */
