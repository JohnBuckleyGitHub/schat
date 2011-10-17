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

#include "net/packets/notices.h"
#include "schat.h"

/*!
 * Ответ на запрос регистрации.
 */
class SCHAT_EXPORT RegReply
{
public:
  RegReply()
  : m_status(Notice::InternalError)
  {}

  RegReply(int status)
  : m_status(status)
  {}

  RegReply(const QString &name)
  : m_status(Notice::OK)
  , m_name(name)
  {}

  inline int status() const { return m_status; }
  inline QString name() const { return m_name; }
  static QString filter(const QString &name);

private:
  int m_status;   ///< Статус регистрации, \sa Notice::StatusCodes.
  QString m_name; ///< Имя присвоенное при регистрации.
};


/*!
 * Ответ на запрос регистрации.
 */
class SCHAT_EXPORT LoginReply
{
public:
  LoginReply()
  : m_status(Notice::InternalError)
  {}

  LoginReply(int status)
  : m_status(status)
  {}

  inline int status() const { return m_status; }
  static QString filter(const QString &name);

private:
  int m_status; ///< Статус регистрации, \sa Notice::StatusCodes.
};

#endif /* ACCOUNTS_H_ */
