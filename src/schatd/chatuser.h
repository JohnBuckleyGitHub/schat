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
#include <QPointer>

#include "connection.h"
#include "schatd.h"

class ChatUser
  : public QObject,
  public boost::enable_shared_from_this<ChatUser>,
  private boost::noncopyable
{
  Q_OBJECT

public:
  ChatUser(const UserData &data);
  ChatUser(const UserData &data, boost::shared_ptr<Connection> connection);
  ~ChatUser();
  inline bool isLocal() const                       { return (bool) m_connection; }
  inline boost::shared_ptr<Connection> connection() { return m_connection; }

public slots:
  void relay(const QByteArray &data);

private:
  boost::shared_ptr<Connection> m_connection;
  UserData m_data;
};

#endif /* CHATUSER_H_ */
