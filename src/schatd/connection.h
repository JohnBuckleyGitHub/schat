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

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>

#include "asio/io_service.hpp"
#include "asio/ip/tcp.hpp"

/*!
 * Represents a single connection from a client.
 */
class Connection
  : public boost::enable_shared_from_this<Connection>,
  private boost::noncopyable
{

public:
  /// Состояние соединения.
  enum State {
    WaitGreeting, ///< Ожидание рукопожатия.
    WaitAccess,   ///< Ожидание подтверждения авторизации.
    Ready,        ///< Соединение активно.
    WaitClose     ///< Соединение закрывается.
  };

  explicit Connection(asio::io_service &ioService);
  ~Connection();

  asio::ip::tcp::socket& socket();
  void close();
  void die();
  void ready();
  void send(const QByteArray &data);
  void start();

private:
  /// Состояние механизма проверки соединения.
  enum PingState {
    WaitPing, ///< Ожидание отправки пинг-запроса (schat::PingInterval).
    WaitPong  ///< Ожидание ответа на пинг-запрос (schat::WaitPong).
  };

  quint16 opcodeGreeting();
  void handleCheck(asio::error_code &err);
  void handleClose();
  void handleReadBody(const asio::error_code &err, int bytes);
  void handleReadHeader(const asio::error_code &err, int bytes);
  void handleWrite(const asio::error_code &err, int bytes);
  void ping(asio::error_code &e);
  void startPing(PingState state, int sec);

  class Private;
  Private* const d;
};

#endif /* CONNECTION_H_ */
