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

#include "emoticonsreader.h"

/*!
 * \class EmoticonsReader
 * \brief Класс для чтения xml-файла смайликов (emoticons.xml формат Kopete).
 */

/*!
 * \brief Конструктор класса EmoticonsReader.
 */
EmoticonsReader::EmoticonsReader(QMap<QString, QStringList> *emoticons)
  : m_emoticons(emoticons)
{
  if (m_emoticons)
    m_emoticons->clear();
}


bool EmoticonsReader::readFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return false;

  setDevice(&file);

  while (!atEnd()) {
    readNext();

    if (isStartElement()) {
      if (name() == "messaging-emoticon-map")
        readMap();
      else
        raiseError(QObject::tr("bad messaging-emoticon-map file"));
    }
  }

  return !error();
}


void EmoticonsReader::readEmoticon(const QString &file)
{
  m_text.clear();

  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (m_emoticons && !file.isEmpty()) {
        if (name() == "string") {
          QString str = readElementText();
          if (!str.isEmpty()) {
            if (m_emoticons->contains(file)) {
              QStringList list = m_emoticons->value(file);
              list << str;
              m_emoticons->insert(file, list);
            }
            else {
              m_emoticons->insert(file, QStringList() << str);
            }
          }
        }
        else
          readUnknownElement();
      }
      else
        readUnknownElement();
    }
  }
}


void EmoticonsReader::readMap()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "emoticon")
        readEmoticon(attributes().value("file").toString());
      else
        readUnknownElement();
    }
  }
}


void EmoticonsReader::readUnknownElement()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
      readUnknownElement();
  }
}
