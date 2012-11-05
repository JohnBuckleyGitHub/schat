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

#include <QtCore>

#include "networkreader.h"


/*!
 * \class NetworkReader
 * \brief Низкоуровневый класс для чтения xml-файла сети.
 *
 * \sa Network
 */

/*!
 * \brief Конструктор класса NetworkReader.
 */
NetworkReader::NetworkReader()
  : m_random(true)
{
}


/** [public]
 *
 */
bool NetworkReader::readFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    raiseError("File not found");
    return false;
  }

  setDevice(&file);

  while (!atEnd()) {
    readNext();

    if (isStartElement()) {
      if (name() == "network" && attributes().value("version") == "1.0")
        readNetwork();
      else
        raiseError("Bad file format or version");
    }
  }

  return !error();
}


/** [private]
 *
 */
void NetworkReader::readMeta()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "name")
        m_name = readElementText();
      else if (name() == "site")
        m_site = readElementText();
      else if (name() == "description")
        m_description = readElementText();
      else if (name() == "key")
        m_key = readElementText();
      else
        readUnknownElement();
    }
  }
}


/** [private]
 *
 */
void NetworkReader::readNetwork()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "meta")
        readMeta();
      else if (name() == "servers") {
        readServers();
      }
      else
        readUnknownElement();
    }
  }
}


/*!
 * Функция читает список серверов сети (элемент host) в список `m_servers`.
 * Адрес сервера может быть вида: "адрес:порт",
 * если порт не указан, используется стандартный 7666.
 */
void NetworkReader::readServers()
{
  if (attributes().value("random") == "false")
    m_random = false;

  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "host") {
        QString t = readElementText();
        QStringList list = t.split(QChar(':'));
        ServerInfo serverInfo;

        if (list.size() == 2) {
          serverInfo.address = list.at(0);
          serverInfo.port = quint16(list.at(1).toUInt());
        } else {
          serverInfo.address = t;
          serverInfo.port = 7666;
        }
        m_servers << serverInfo;
      }
      else
        readUnknownElement();
    }
  }
}


/** [private]
 *
 */
void NetworkReader::readUnknownElement()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
      readUnknownElement();
  }
}
