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

#include <QtCore>
#include <boost/bind.hpp>

#include "chatserver.h"

/*!
 * Construct the server to listen on the specified TCP address and port.
 */
ChatServer::ChatServer(const QString &address, quint16 port, int poolSize)
  : m_workerPool(poolSize),
  m_acceptor(m_workerPool.getIoService()),
  newConnection(new Connection(m_workerPool.getIoService()))
{
  qDebug() << "ChatServer()" << this;

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  asio::ip::tcp::resolver resolver(m_acceptor.io_service());
  asio::ip::tcp::resolver::query query(address.toLatin1().constData(), QString().setNum(port).toLatin1().constData());
  asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  m_acceptor.open(endpoint.protocol());
  m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  m_acceptor.bind(endpoint);
  m_acceptor.listen();
  m_acceptor.async_accept(newConnection->socket(),
      boost::bind(&ChatServer::handleAccept, this,
      asio::placeholders::error));
}


/*!
 * Run the server's io_service loop.
 */
void ChatServer::run()
{
  qDebug() << "ChatServer::run()" << this;

  m_workerPool.run();
}


/*!
 * Stop the server.
 */
void ChatServer::stop()
{
  qDebug() << "ChatServer::stop()" << this;

  m_workerPool.stop();
}


/*!
 * Handle completion of an asynchronous accept operation.
 */
void ChatServer::handleAccept(const asio::error_code &e)
{
  qDebug() << "ChatServer::handleAccept()" << this;

  if (!e) {
    newConnection->start();
    newConnection.reset(new Connection(m_workerPool.getIoService()));
    m_acceptor.async_accept(newConnection->socket(),
        boost::bind(&ChatServer::handleAccept, this,
          asio::placeholders::error));
  }
}
