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

#ifndef WORKERTHREAD_H_
#define WORKERTHREAD_H_

#include <QThread>
#include <boost/shared_ptr.hpp>

#include "asio/io_service.hpp"

/*!
 * \brief Рабочий поток сервера чата.
 *
 * Число рабочих потоков рекомендуется устанавливать равным числу ядер/процессоров.
 */
class WorkerThread : public QThread
{
public:
  WorkerThread(asio::io_service &io, QObject *parent = 0);
  ~WorkerThread();

  asio::io_service& io();
  void run();
  void stop();

private:
  class Private;
  Private* const d;
};

#endif /* WORKERTHREAD_H_ */
