/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#ifndef CHATUSER_H_
#define CHATUSER_H_

#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "connection.h"
#include "schatd.h"

/*!
 * Базовая информация об пользователе.
 */
//struct UserData {
//  QString nick;      ///< Ник.
//  QString fullName;  ///< Полное имя.
//  QString userAgent; ///< Идентификация клиента пользователя.
//  QString byeMsg;    ///< Сообщение о выходе.
//  QString host;      ///< IP-адрес.
//  quint8 gender;     ///< Пол.
//  int protocol;      ///< Версия протокола.
//};


/*!
 * Базовый комуникационный класс хранящий иноформацию о пользователе.
 */
class ChatUser
  : public boost::enable_shared_from_this<ChatUser>,
  private boost::noncopyable
{
public:
  ChatUser(const UserData &data);
  ChatUser(const UserData &data, boost::shared_ptr<Connection> connection);
  ~ChatUser();
  inline bool isLocal() const                       { return (bool) m_connection; }
  inline boost::shared_ptr<Connection> connection() { return m_connection; }

private:
  boost::shared_ptr<Connection> m_connection;
  UserData m_data;
};

#endif /* CHATUSER_H_ */
