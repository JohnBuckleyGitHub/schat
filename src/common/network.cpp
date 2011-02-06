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

#include <QtCore>

#include "network.h"

const QString Network::failBackServer = "schat.impomezia.com";
const quint16 Network::failBackPort   = 7666;

/*!
 * \brief Конструктор класса Network.
 */
Network::Network(QObject *parent)
  : QObject(parent),
  m_failBack(true),
  m_random(true),
  m_single(false),
  m_networksPath(QCoreApplication::applicationDirPath() + "/networks")
{
  qsrand(QDateTime(QDateTime::currentDateTime()).toTime_t());
}


/*!
 * \brief Конструктор класса Network.
 *
 * \param paths  Пути в которых будут икаться файлы сети.
 * \param parent Указатель на родительский объект.
 */
Network::Network(const QStringList &paths, QObject *parent)
  : QObject(parent),
  m_failBack(true),
  m_random(true),
  m_single(false),
  m_networksPath(paths)
{
  qsrand(QDateTime(QDateTime::currentDateTime()).toTime_t());
}


/*!
 * Получение списка сервером, входная строка является записью в конфигурационном файле,
 * если файл найден, вызывается функция `fromFile()` если нет `fromString()`.
 */
bool Network::fromConfig(const QString &s)
{
  if (QFileInfo(s).isAbsolute()) {
    if (QFile::exists(s))
      return fromFile(s);
    else
      return fromConfig(QFileInfo(s).fileName());
  }

  foreach (QString path, m_networksPath) {
    if (QFile::exists(path + "/" + s))
      return fromFile(path + "/" + s);
  }

  return fromString(s);
}


/*!
 * Читает файл сети.
 *
 * \param f Имя файла без пути.
 */
bool Network::fromFile(const QString &f)
{
  NetworkReader reader;
  m_servers.clear();

  QString file;
  if (QFileInfo(f).isRelative()) {
    for (int i = 0; i < m_networksPath.size(); ++i) {
      if (QFile::exists(m_networksPath.at(i) + "/" + f)) {
        file = m_networksPath.at(i) + "/" + f;
        break;
      }
    }
  }
  else {
    file = f;
  }

  if (!file.isEmpty() && reader.readFile(file)) {
    m_valid       = true;
    m_network     = true;
    m_description = reader.description().left(MaxDesc);
    m_name        = reader.networkName().left(MaxName);
    m_site        = reader.site().left(MaxSite);
    m_key         = reader.key().left(MaxKey);
    m_servers     = reader.servers();
    m_random      = reader.isRandom();
    m_file        = file;
  }
  else {
    m_valid   = false;
    m_network = false;
  }
  m_error = reader.errorString();
  return m_valid;
}


/** [public]
 * Получение адреса и порта сервера из строки.
 */
bool Network::fromString(const QString &s)
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
  return m_valid;
}


/*!
 * Возвращает строку для записи в конфигурационный файл,
 * это может быть именем файла сети либо в случае одиночного сервера,
 * парой "адрес:порт".
 */
QString Network::config() const
{
  if (m_valid) {
    if (m_network)
      return QFileInfo(m_file).fileName();
    else
      return m_servers.at(0).address + ':' + QString().setNum(m_servers.at(0).port);
  }
  else
    return failBackServer + ':' + QString().setNum(failBackPort);
}


/*!
 * Функция возвращает структуру \a ServerInfo.
 * Если серверов больше одного, то возвращается случайный сервер,
 * при этом функция не допускает выдачи подряд одного и того же сервера.
 */
ServerInfo Network::server() const
{
  if (m_servers.count() == 0) {
    if (m_failBack)
      return failBack();
    else {
      ServerInfo info;
      info.address = "";
      info.port    = 7666;
      return info;
    }
  }

  if (m_servers.count() == 1)
    return m_servers.at(0);

  if (m_single)
    return m_servers.at(0);

  int index = 0;
  static int prevIndex;
  static bool init;

  if (m_random) {
    if (init) {
      do {
        index = qrand() % m_servers.count();
      } while (index == prevIndex);
    }
    else {
      init = true;
      index = qrand() % m_servers.count();
    }
  }
  else {
    if (init) {
      if (prevIndex < m_servers.count() - 1)
        index = ++prevIndex;
      else
        index = 0;
    }
    else {
      init = true;
    }
  }

  prevIndex = index;

  return m_servers.at(index);
}


/*!
 * \brief Статическая функция возвращающая структуру \a ServerInfo по умолчанию.
 */
ServerInfo Network::failBack()
{
  ServerInfo info;
  info.address = failBackServer;
  info.port    = failBackPort;
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

  return failBackPort;
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
