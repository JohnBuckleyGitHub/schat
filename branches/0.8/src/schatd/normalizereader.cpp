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

#include <QFile>
#include <QDebug>

#include "normalizereader.h"

/*!
 * Конструктор класса NormalizeReader.
 */
NormalizeReader::NormalizeReader(QHash<QChar, QChar> &normalize)
  : m_normalize(normalize)
{
}


/*!
 * Чтение файла.
 */
bool NormalizeReader::readFile(const QString &fileName)
{
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return false;

  setDevice(&file);
  m_normalize.clear();

  while (!atEnd()) {
    readNext();

    if (isStartElement()) {
      if (name() == "normalize" && attributes().value("version") == "1.0")
        readChar();
      else
        raiseError("BAD FILE FORMAT OR VERSION");
    }
  }

  return !error();
}


void NormalizeReader::readChar()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "char") {
        QString n = attributes().value("n").toString();
        QString k = readElementText();
        if (n.size() > 0 && k.size() > 0) {
          if (!m_normalize.contains(k.at(0)))
            m_normalize.insert(k.at(0), n.at(0));
        }
      }
      else
        readUnknownElement();
    }
  }
}


/*!
 * Чтение неизвестного элемента.
 */
void NormalizeReader::readUnknownElement()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
      readUnknownElement();
  }
}
