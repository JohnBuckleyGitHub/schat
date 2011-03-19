/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

class Core;
class Worker;

class WorkerThread : public QThread
{
  Q_OBJECT

public:
  WorkerThread(Core *parent);
  ~WorkerThread();
  inline QList<Worker*> workers() const { return m_workers; };

signals:
  void workersStarted();

protected:
  void run();

private:
  Core *m_core;
  QList<Worker*> m_workers;
};

#endif /* WORKERTHREAD_H_ */
