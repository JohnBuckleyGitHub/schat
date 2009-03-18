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

#ifndef WORKERPOOL_H_
#define WORKERPOOL_H_

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <QList>

#include "asio/asio.hpp"

/*!
 * A pool of io_service objects.
 */
class WorkerPool
  : private boost::noncopyable
{
public:
  explicit WorkerPool(int poolSize);
  asio::io_service& getIoService();
  void run();
  void stop();

private:
  typedef boost::shared_ptr<asio::io_service> ioServicePtr;
  typedef boost::shared_ptr<asio::io_service::work> workPtr;

  int m_nextIoService;              ///< The next io_service to use for a connection.
  QList<ioServicePtr> m_ioServices; ///< The pool of io_services.
  QList<workPtr> m_work;            ///< The work that keeps the io_services running.
};

#endif /* WORKERPOOL_H_ */
