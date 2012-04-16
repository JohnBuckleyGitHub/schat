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

#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include "schat.h"
#include "acl/Groups.h"

#include <QString>

/*!
 * Информация об аккаунте пользователя.
 */
class SCHAT_EXPORT Account
{
public:
  Account()
  : m_channel(0)
  , m_date(0)
  , m_id(0)
  {}

  bool isValid() const;

  inline const QByteArray& cookie() const   { return m_cookie; }
  inline const QByteArray& password() const { return m_password; }
  inline const QString& name() const        { return m_name; }
  inline qint64 channel() const             { return m_channel; }
  inline qint64 date() const                { return m_date; }
  inline qint64 id() const                  { return m_id; }

  inline void setChannel(qint64 channel)                { m_channel = channel; }
  inline void setCookie(const QByteArray &cookie)       { m_cookie = cookie; }
  inline void setDate(qint64 date)                      { m_date = date; }
  inline void setId(qint64 id)                          { m_id = id; }
  inline void setName(const QString &name)              { m_name = name; }
  inline void setPassword(const QByteArray &password)   { m_password = password; }

  inline const Groups& groups() const { return m_groups; }
  inline Groups& groups()             { return m_groups; }

private:
  Groups m_groups;       ///< Группы пользователя.
  QByteArray m_cookie;   ///< Сookie пользователя.
  QByteArray m_password; ///< Пароль пользователя.
  qint64 m_channel;      ///< Идентификатор канала в таблице channels.
  qint64 m_date;         ///< Дата обновления.
  qint64 m_id;           ///< Идентификатор в таблице accounts.
  QString m_name;        ///< Зарегистрированное имя пользователя.
};

#endif /* ACCOUNT_H_ */
