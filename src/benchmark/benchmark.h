/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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
#include <QThread>

class AbstractSettings;
class Network;

/*!
 * \brief Класс для стресс-тестирования.
 */
class Benchmark : public QThread
{
  Q_OBJECT

public:
  Benchmark(QObject *parent = 0);
  ~Benchmark();

signals:
  void rejected(int count);
  void started(int count);

private slots:
  void accessDenied(quint16 reason);
  void connectToHost();

protected:
  void run();

private:
  AbstractSettings *m_settings; ///< Настройки.
  int m_connectInterval;        ///< Интервал подключения (ConnectInterval), по умолчанию 200 мс.
  int m_count;                  ///< Счётчик созданных подключений.
  int m_rejected;               ///< Счётчик клиентов которым отказано в подключении.
  int m_usersCount;             ///< Число подключений (UsersCount), по умолчанию 10.
  Network *m_network;           ///< Извлекает данные для подключения из строки.
  QString m_nickPrefix;         ///< Префикс имён пользователей (NickPrefix), по умолчанию "test_".
  QString m_serverAddr;         ///< Адрес сервера для тестирования (Network), по умолчанию "192.168.238.1:7667".
};

#endif /* BENCHMARK_H_ */
