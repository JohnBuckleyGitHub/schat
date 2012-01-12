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
{
}


/** [public]
 * 
 */
bool NetworkReader::readFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return false;
  
  setDevice(&file);

  while (!atEnd()) {
    readNext();

    if (isStartElement()) {
      if (name() == "network" && attributes().value("version") == "1.0")
        readNetwork();
      else
        raiseError(QObject::tr("The file is not an NETWORK version 1.0 file."));
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
      else if (name() == "servers")
        readServers();
      else
        readUnknownElement();
    }
  }
}


/** [private]
 * Функция читает список серверов сети (элемент host) в список `m_servers`.
 * Адрес сервера может быть вида: "адрес:порт",
 * если порт не указан, используется стандартный 7666.
 */
void NetworkReader::readServers()
{
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