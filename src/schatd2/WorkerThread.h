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

#include <QHash>
#include <QStringList>
#include <QThread>

class SimpleSocket;
class Worker;

class WorkerThread : public QThread
{
  Q_OBJECT

public:
  WorkerThread(const QStringList &listen, QObject *core, QObject *parent = 0);

signals:
  void ready(QObject *listener);

protected:
  void run();

private:
  QObject *m_core;
  QStringList m_listen;
};


/*!
 * Слушатель событий от ядра чата.
 */
class WorkerEventListener : public QObject
{
  Q_OBJECT

public:
  WorkerEventListener(QObject *core, QObject *parent = 0);
  ~WorkerEventListener();
  bool add(const QString &listen);
  inline QObject *core() { return m_core; }
  inline quint64 counter() const { return m_counter; }
  inline SimpleSocket *socket(quint64 id) { return m_sockets.value(id); }
  inline void add(SimpleSocket *socket) { m_sockets[m_counter] = socket; m_counter++; }
  void release(quint64 id);

protected:
  void customEvent(QEvent *event);

private:
  QHash<quint64, SimpleSocket*> m_sockets; ///< Таблица сокетов.
  QList<Worker*> m_workers;                ///< Список объектов Worker.
  QObject *m_core;
  quint64 m_counter;
};

#endif /* WORKERTHREAD_H_ */
