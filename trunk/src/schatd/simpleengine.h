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

#ifndef SIMPLEENGINE_H_
#define SIMPLEENGINE_H_

#include <QObject>

class WorkerThread;

/*!
 * \brief Базовые загрузочные опции сервера.
 *
 * Эти опции не могут быть изменены на протяжении всей работы сервера.
 */
struct BootOptions
{
  int poolSize;    ///< Количество рабочих потоков.
  QString address; ///< Адрес сервера.
  quint16 port;    ///< Порт сервера.
};


/*!
 * \brief Сервер чата.
 *
 * Класс полностью включает в себя функциональность сервера чата.
 */
class SimpleEngine : public QObject
{
  Q_OBJECT

public:
  SimpleEngine(const BootOptions &boot, QObject *parent = 0);
  ~SimpleEngine();
  inline static SimpleEngine *instance() { return m_self; }
  void run();
  void stop();
  WorkerThread* worker();

private:
  class Private;
  Private* const d;

  static SimpleEngine *m_self;
};

#endif /* SIMPLEENGINE_H_ */
