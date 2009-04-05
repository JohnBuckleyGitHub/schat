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
#include <boost/shared_ptr.hpp>

#include "workerpool.h"
#include "workerthread.h"

/*!
 * Construct the \b io_service pool.
 *
 * Give all the \b io_services work to do so that their run() functions will not
 * exit until they are explicitly stopped.
 */
WorkerPool::WorkerPool(int poolSize)
  : m_nextIoService(0)
{
  qDebug() << "WorkerPool()" << this;

  if (poolSize <= 1)
    poolSize = 1;

  for (int i = 0; i < poolSize; ++i) {
    ioServicePtr io_service(new asio::io_service);
    workPtr work(new asio::io_service::work(*io_service));
    m_ioServices.append(io_service);
    m_work.append(work);
  }
}


/*!
 * Get an io_service to use.
 * Use a round-robin scheme to choose the next \b io_service to use.
 */
asio::io_service& WorkerPool::getIoService()
{
//  qDebug() << "WorkerPool::getIoService()" << this;

  asio::io_service& ioService = *m_ioServices[m_nextIoService];
  ++m_nextIoService;
  if (m_nextIoService == m_ioServices.size())
    m_nextIoService = 0;

  return ioService;
}


/*!
 * Run all \b io_service objects in the pool.
 *
 * Create a pool of threads to run all of the io_services.
 */
void WorkerPool::run()
{
  qDebug() << "WorkerPool::run()" << this << m_ioServices.size();

//  QList<boost::shared_ptr<asio::thread> > threads;

  for (int i = 0; i < m_ioServices.size(); ++i) {
//    boost::shared_ptr<asio::thread> thread(new asio::thread(boost::bind(&asio::io_service::run, m_ioServices[i])));
//    threads.append(thread);
    WorkerThread *thread = new WorkerThread(*m_ioServices[i]);
    thread->start();
  }

  qDebug() << "[1]";

  // Wait for all threads in the pool to exit.
//  for (int i = 0; i < threads.size(); ++i) {
//    qDebug() << i;
////    threads[i]->join();
//  }

  qDebug() << "[2]";
}


/*!
 * Stop all \b io_service objects in the pool.
 * Explicitly stop all io_services.
 */
void WorkerPool::stop()
{
  // Explicitly stop all io_services.
  for (int i = 0; i < m_ioServices.size(); ++i)
    m_ioServices[i]->stop();
}
