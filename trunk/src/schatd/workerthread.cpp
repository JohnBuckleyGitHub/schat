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

#include <boost/bind.hpp>
#include <QtCore>

#define SCHAT_DEBUG

#include "asio/asio.hpp"
#include "workerthread.h"

/*!
 * \brief Приватный D-класс для класса Connection.
 */
class WorkerThread::Private
{
public:
  Private(asio::io_service &io)
  : jobTimer(io),
  io(io)
  {}

  ~Private() {}
  void handleJob(asio::error_code &err);

  asio::deadline_timer jobTimer;
  asio::io_service &io;          ///< Сервис.
};


void WorkerThread::Private::handleJob(asio::error_code &err)
{
//  qDebug() << "handleJob()";
  if (!err) {
    jobTimer.expires_from_now(boost::posix_time::millisec(100));
    jobTimer.async_wait(boost::bind(&WorkerThread::Private::handleJob, this, _1));
  }
}


/*!
 * Конструктор класса WorkerThread.
 */
WorkerThread::WorkerThread(asio::io_service &io, QObject *parent)
  : QThread(parent), d(new Private(io))
{
}


WorkerThread::~WorkerThread()
{
  delete d;
}


asio::io_service& WorkerThread::io()
{
  return d->io;
}


/*!
 * Запуск цикла обработки событий.
 */
void WorkerThread::run()
{
//  asio::error_code err;
//  d->handleJob(err);

  d->io.run();
}


/*!
 * Остановка рабочего потока.
 */
void WorkerThread::stop()
{
  d->io.stop();
}
