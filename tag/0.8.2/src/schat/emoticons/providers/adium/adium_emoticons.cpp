/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
 *
 * Base class AdiumEmoticons
 * Copyright © 2008 by Carlo Segato <brandon.ml@gmail.com>
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

#include "adium_emoticons.h"

/*!
 * Конструктор класса AdiumEmoticons.
 */
AdiumEmoticons::AdiumEmoticons(QObject *parent)
  : EmoticonsProvider(parent)
{
}


bool AdiumEmoticons::loadTheme(const QString &path)
{
  EmoticonsProvider::loadTheme(path);

  QFile file(path);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return false;

  m_reader.setDevice(&file);

  while (!m_reader.atEnd()) {
    m_reader.readNext();

    if (m_reader.isStartElement()) {
      if (m_reader.name() == "plist") {
        clearEmoticonsMap();

        while (!m_reader.atEnd()) {
          m_reader.readNext();

          if (m_reader.isEndElement())
            break;

          if (m_reader.isStartElement()) {
            if (m_reader.name().toString() == "dict")
              while (!m_reader.atEnd()) {
                m_reader.readNext();

                if (m_reader.isEndElement())
                  break;

                if (m_reader.isStartElement()) {
                  if (m_reader.name().toString() == "dict")
                    readDict();
                  else
                    readUnknownElement();
                }
              }
            else
              readUnknownElement();
          }
        }

      }
      else
        m_reader.raiseError("bad plist file");
    }
  }

  return !m_reader.error();
}


void AdiumEmoticons::readDict()
{
  clearEmoticonsMap();

  while (!m_reader.atEnd()) {
    m_reader.readNext();

    if (m_reader.isEndElement())
      break;

    if (m_reader.isStartElement()) {

      if (m_reader.name() == "key")
        m_name = m_reader.readElementText();
      else if (m_reader.name() == "dict" && !m_name.isEmpty())
        readEmo();
      else
        readUnknownElement();
    }
  }
}


void AdiumEmoticons::readEmo()
{
  QStringList sl;

  while (!m_reader.atEnd()) {
    m_reader.readNext();

    if (m_reader.isEndElement())
      break;

    if (m_reader.isStartElement()) {
      if (m_reader.name() == "array") {
        while (!m_reader.atEnd()) {
          m_reader.readNext();

          if (m_reader.isEndElement())
            break;

          if (m_reader.isStartElement()) {
            if (m_reader.name() == "string") {
              QString text = m_reader.readElementText();
              if (!text.isEmpty())
                sl << text;
            }
            else
              readUnknownElement();
          }
        }

      }
      else
        readUnknownElement();
    }
  }

  m_name = themePath() + '/' + m_name;

  if (QFile::exists(m_name)) {
    addEmoticonIndex(m_name, sl);
    addEmoticonsMap(m_name, sl);
    m_name.clear();
  }
}


void AdiumEmoticons::readUnknownElement()
{
  while (!m_reader.atEnd()) {
    m_reader.readNext();

    if (m_reader.isEndElement())
      break;

    if (m_reader.isStartElement())
      readUnknownElement();
  }
}
