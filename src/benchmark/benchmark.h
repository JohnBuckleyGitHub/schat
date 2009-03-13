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

#ifndef BENCHMARK_H_
#define BENCHMARK_H_

#include <QObject>
#include <QStringList>

class AbstractSettings;
class Network;

/*!
 * \brief Класс для стресс-тестирования.
 */
class Benchmark : public QObject {
  Q_OBJECT

public:
  Benchmark(QObject *parent = 0);

private slots:
  void accessDenied(quint16 reason);
  void connectToHost();
  void init();

private:

  AbstractSettings *m_settings;
  int m_connectInterval;
  int m_count;
  int m_usersCount;
  Network *m_network;
  QString m_nickPrefix;
  QString m_serverAddr;
};

#endif /* BENCHMARK_H_ */
