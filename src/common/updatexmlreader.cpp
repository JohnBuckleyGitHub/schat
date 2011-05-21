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

#include "updatexmlreader.h"

/*!
 * Конструктор класса UpdateXmlReader.
 */
UpdateXmlReader::UpdateXmlReader()
{
}


/*!
 * Проверка корректности чтения.
 * Функция должна вызываться после readFile(const QString &fileName).
 */
bool UpdateXmlReader::isValid() const
{
  if (error())
    return false;

  #ifndef SCHAT_NO_UPDATE
    if (m_version.isEmpty() || m_files.isEmpty())
  #else
    if (m_version.isEmpty())
  #endif
    return false;

  return true;
}


/*!
 * Чтение файла.
 */
bool UpdateXmlReader::readFile(const QString &fileName)
{
  #ifndef SCHAT_NO_UPDATE
    m_files.clear();
    m_baseUrl.clear();
  #endif
  m_version.clear();

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return false;

  setDevice(&file);

  while (!atEnd()) {
    readNext();

    if (isStartElement()) {
      if (name() == "updates" && attributes().value("version") == "1.1")
        readUpdates();
      else
        raiseError("BAD FILE FORMAT OR VERSION");
    }
  }

  return !error();
}


/*!
 * Проверка правильности структуры VersionInfo.
 *
 * \return \a true успешная проверка, \a false ошибка.
 */
bool UpdateXmlReader::isValid(const VersionInfo &version)
{
  if (version.level == 0)
    return false;

  if (version.type.isEmpty() || version.version.isEmpty())
    return false;

  return true;
}


/*!
 * Проверка правильности структуры FileInfo.
 *
 * \return \a true успешная проверка, \a false ошибка.
 */
#ifndef SCHAT_NO_UPDATE
bool UpdateXmlReader::isValid(const FileInfo &file)
{
  if (file.size == 0 || file.level == 0)
    return false;

  if (file.md5.isEmpty() || file.type.isEmpty() || file.name.isEmpty())
    return false;

  return true;
}
#endif


/*!
 * Чтение элемента meta.
 */
void UpdateXmlReader::readMeta()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "version") {
        VersionInfo info;
        info.level   = attributes().value("level").toString().toInt();
        info.type    = attributes().value("type").toString();
        info.version = readElementText();

        if (isValid(info))
          m_version << info;
      }
      else
        readUnknownElement();
    }
  }
}


/*!
 * Чтение неизвестного элемента.
 */
void UpdateXmlReader::readUnknownElement()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement())
      readUnknownElement();
  }
}


/*!
 * Чтение корневого элемента.
 */
void UpdateXmlReader::readUpdates()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "meta")
        readMeta();

      #ifndef SCHAT_NO_UPDATE
      else if (name() == "files") {
        QUrl url(attributes().value("baseurl").toString());
        if (url.isValid()) {
          m_baseUrl = url.toString();
          if (!m_baseUrl.endsWith("/"))
            m_baseUrl += "/";
        }
        readFiles();
      }
      #endif

      else
        readUnknownElement();
    }
  }
}


/*!
 * Чтение элемента cumulative.
 */
#ifndef SCHAT_NO_UPDATE
void UpdateXmlReader::readCumulative()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "file") {
        FileInfo fileInfo;
        fileInfo.type  = attributes().value("type").toString();
        fileInfo.level = attributes().value("level").toString().toInt();
        fileInfo.size  = attributes().value("size").toString().toULongLong();
        fileInfo.md5   = attributes().value("md5").toString();
        fileInfo.name  = readElementText();

        if (isValid(fileInfo))
          m_files.insert(fileInfo.level, fileInfo);
      }
      else
        readUnknownElement();
    }
  }
}


/*!
 * Чтение элемента files.
 */
void UpdateXmlReader::readFiles()
{
  while (!atEnd()) {
    readNext();

    if (isEndElement())
      break;

    if (isStartElement()) {
      if (name() == "cumulative")
        readCumulative();
      else
        readUnknownElement();
    }
  }
}
#endif
