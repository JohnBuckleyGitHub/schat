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

#ifndef CHATSERVER_H_
#define CHATSERVER_H_

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <QString>

#include "asio/asio.hpp"
#include "connection.h"
#include "workerpool.h"

/*!
 * The top-level class of the chat server.
 */
class ChatServer
  : private boost::noncopyable
{
public:
  explicit ChatServer(const QString &address, quint16 port, int poolSize);

  void run();
  void stop();

private:
  void handleAccept(const asio::error_code& e);

  WorkerPool m_workerPool;            ///< The pool of io_service objects used to perform asynchronous operations.
  asio::ip::tcp::acceptor m_acceptor; ///< Acceptor used to listen for incoming connections.
  ConnectionPtr newConnection;        ///< The next connection to be accepted.
};

#endif /* CHATSERVER_H_ */
