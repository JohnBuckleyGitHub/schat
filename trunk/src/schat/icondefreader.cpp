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

#include "icondefreader.h"

/*!
 * \class IconDefReader
 * \brief Класс для чтения xml-файла смайликов (icondef.xml формат Psi).
 */

/*!
 * \brief Конструктор класса IconDefReader.
 */
IconDefReader::IconDefReader(QMap<QString, QStringList> *emoticons)
  : m_emoticons(emoticons)
{
  if (m_emoticons)
    m_emoticons->clear();
  
  m_refresh = 0;
}


bool IconDefReader::readFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return false;

  setDevice(&file);

  while (!atEnd()) {
    readNext();

    if (isStartElement()) {
      if (name() == "icondef")
        readIcondef();
      else
        raiseError(QObject::tr("bad icondef file"));
    }
  }

  return !error();
}


void IconDefReader::readIcon()
{
  m_text.clear();

  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (m_emoticons) {
        if (name() == "text") {
          QString text = readElementText();
          if (!text.isEmpty())
            m_text << text;
        }
        else if (name() == "object") {
          QString object = readElementText();
          if (!m_text.isEmpty() && !object.isEmpty())
            m_emoticons->insert(object, m_text);
        }
        else
          readUnknownElement();
      }
      else
        readUnknownElement();
    }
  }
}


void IconDefReader::readIcondef()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "meta")
        readMeta();
      else if (name() == "icon")
        readIcon();
      else
        readUnknownElement();
    }
  }
}


void IconDefReader::readMeta()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "refresh")
        m_refresh = readElementText().toInt();
      else
        readUnknownElement();
    }
  }
}


void IconDefReader::readUnknownElement()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
      readUnknownElement();
  }
}
