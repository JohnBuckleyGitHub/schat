/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui>

#include "network.h"


/** [public]
 * 
 */
Network::Network()
{
  m_networksPath = qApp->applicationDirPath() + "/networks/";
  qsrand(QDateTime(QDateTime::currentDateTime()).toTime_t());
}


/** [public]
 * Возвращает строку для записи в конфигурационный файл,
 * это может быть именем файла сети либо в случае одиночного сервера,
 * парой "адрес:порт".
 */
QString Network::config() const
{
  if (m_valid) {
    if (m_network)
      return m_file;
    else
      return m_servers.at(0).address + ':' + QString().setNum(m_servers.at(0).port);
  }
  else
    return FailBackServer + ':' + QString().setNum(FailBackPort);
}


/** [public]
 * Функция возвращает структуру `ServerInfo`.
 * Если серверов больше одного, то возвращается случайный сервер,
 * при этом функция не допускает выдачи подряд одного и того же сервера.
 */
ServerInfo Network::server() const
{
  if (m_servers.count() == -1)
    return failBack();
  
  if (m_servers.count() == 1)
    return m_servers.at(0);
  
  int index;
  static int prevIndex;
  static bool init;
  
  if (init) {
    do {
      index = qrand() % (m_servers.size() - 1);
    } while (index == prevIndex);
  }
  else {
    init = true;
    index = qrand() % (m_servers.size() - 1);
  }
  
  prevIndex = index;
  
  return m_servers.at(index);
}


/** [public] static
 * Статическая функция возвращающая структуру `ServerInfo` по умолчанию.
 */
ServerInfo Network::failBack()
{
  ServerInfo info;
  info.address = FailBackServer;
  info.port    = FailBackPort;
  return info;
}


/** [public]
 * Возвращает адрес порта текущего сервера, функция возвращает верное значение
 * только в случае одиночного сервера, в противном случае возвращается `FailBackPort`.
 */
quint16 Network::port() const
{
  if (m_valid && m_servers.size() == 1)
    return m_servers.at(0).port;

  return FailBackPort;
}


/** [public] static
 * Статическая функция, возвращает структура ServerInfo на основе
 * строки формата "адрес:порт", если входная строка не удовлетворяет
 * этому условию возвращается структура со стандартными значениями сервера и порта
 * (глобальные переменные `FailBackServer` и `FailBackPort`).
 */
ServerInfo Network::serverInfo(const QString &s)
{
  QStringList list = s.split(QChar(':'));
  
  if (list.size() == 2) {
    ServerInfo info;
    info.address = list.at(0);
    info.port    = quint16(list.at(1).toUInt());
    return info;
  } else
    return failBack();
}


/** [public]
 * Получение списка сервером, входная строка является записью в конфигурационном файле,
 * если файл найден, вызывается функция `fromFile()` если нет `fromString()`.
 */
void Network::fromConfig(const QString &s)
{
  if (QFile::exists(m_networksPath + s))
    fromFile(s);
  else
    fromString(s);  
}


/** [public]
 * Парсинг файла сети.
 */
void Network::fromFile(const QString &file)
{
  NetworkReader reader;
  m_servers.clear();
  
  if (reader.readFile(m_networksPath + file)) {
    m_valid       = true;
    m_network     = true;
    m_description = reader.description();
    m_name        = reader.networkName();
    m_site        = reader.site();
    m_servers     = reader.servers();
    m_file        = file;
  }
  else {
    m_valid   = false;
    m_network = false;
  }
}


/** [public]
 * Получение адреса и порта сервера из строки.
 */
void Network::fromString(const QString &s)
{
  m_servers.clear();
  
  QStringList list = s.split(QChar(':'));
  if (list.size() == 2) {
    m_valid   = true;
    m_network = false;
    ServerInfo info;
    info.address = list.at(0);
    info.port    = quint16(list.at(1).toUInt());
    m_servers << info;
  }
  else {
    m_valid   = false;
    m_network = false;
  }
}
