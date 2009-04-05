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

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <QtCore>

#include "asio/asio.hpp"
#include "simpleengine.h"
#include "workerthread.h"
#include "connection.h"

SimpleEngine *SimpleEngine::m_self = 0;

/*!
 * \brief Приватный D-класс для класса SimpleEngine.
 */
class SimpleEngine::Private
{
public:
  Private(const BootOptions &boot, SimpleEngine *parent);
  ~Private()
  {
    delete acceptor;
  }

  void handleAccept(const asio::error_code &err);
  WorkerThread* worker();

  asio::ip::tcp::acceptor *acceptor; ///< Acceptor used to listen for incoming connections.
  boost::shared_ptr<Connection> connection; ///< The next connection to be accepted.
  const BootOptions &boot;
  int nextWorker;                   ///< The next io_service to use for a connection.
  QList<WorkerThread *> workers;
  SimpleEngine *q;

private:
  QList<boost::shared_ptr<asio::io_service::work> > works; ///< The work that keeps the io_services running.
  QList<boost::shared_ptr<asio::io_service> > ios;         ///< The pool of io_services.
};


/*!
 * Конструктор класса SimpleEngine::Private.
 */
SimpleEngine::Private::Private(const BootOptions &boot, SimpleEngine *parent)
  : connection(),
  boot(boot),
  nextWorker(0),
  q(parent)
{
  for (int i = 0; i < boot.poolSize; ++i) {
    boost::shared_ptr<asio::io_service> io_service(new asio::io_service);
    boost::shared_ptr<asio::io_service::work> work(new asio::io_service::work(*io_service));
    ios.append(io_service);
    works.append(work);
    workers.append(new WorkerThread(*io_service, q));
  }

  acceptor = new asio::ip::tcp::acceptor(*ios.at(0));
  asio::ip::tcp::resolver resolver(acceptor->io_service());
  asio::ip::tcp::resolver::query query(boot.address.toLatin1().constData(), QString().setNum(boot.port).toLatin1().constData());
  asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor->open(endpoint.protocol());
  acceptor->set_option(asio::ip::tcp::acceptor::reuse_address(true));
  acceptor->bind(endpoint);
  acceptor->listen();

  connection.reset(new Connection(workers.at(0)));
  acceptor->async_accept(connection->socket(),
      boost::bind(&SimpleEngine::Private::handleAccept, this, _1));
}


/*!
 * Handle completion of an asynchronous accept operation.
 */
void SimpleEngine::Private::handleAccept(const asio::error_code &err)
{
  if (!err) {
    connection->start();
    connection.reset(new Connection(worker()));
    acceptor->async_accept(connection->socket(),
        boost::bind(&SimpleEngine::Private::handleAccept, this, _1));
  }
}


WorkerThread* SimpleEngine::Private::worker()
{
  WorkerThread *thread = workers[nextWorker];
  ++nextWorker;
  if (nextWorker == workers.size())
    nextWorker = 0;

  return thread;
}


/*!
 * Конструктор класса SimpleEngine.
 */
SimpleEngine::SimpleEngine(const BootOptions &boot, QObject *parent)
  : QObject(parent), d(new Private(boot, this))
{
  qDebug() << this;

  Q_ASSERT(!m_self);
  m_self = this;
}


SimpleEngine::~SimpleEngine()
{
  qDebug() << "~" << this;
  delete d;
}


void SimpleEngine::run()
{
  for (int i = 0; i < d->workers.size(); ++i)
    d->workers[i]->start();
}


void SimpleEngine::stop()
{
  for (int i = 0; i < d->workers.size(); ++i)
    d->workers[i]->stop();
}
