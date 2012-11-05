/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
 *
 * Base class XmppEmoticons
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

#include "xmpp_emoticons.h"

#include <QFile>
#include <QFileInfo>
#include <QImageReader>

/*!
 * Конструктор класса XmppEmoticons.
 */
XmppEmoticons::XmppEmoticons(QObject *parent)
  : EmoticonsProvider(parent)
{
  m_mime << "image/png" << "image/gif" << "image/bmp" << "image/jpeg";
}


bool XmppEmoticons::loadTheme(const QString &path)
{
  EmoticonsProvider::loadTheme(path);

  QFile file(path);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return false;

  m_reader.setDevice(&file);

  while (!m_reader.atEnd()) {
    m_reader.readNext();

    if (m_reader.isStartElement()) {
      if (m_reader.name() == "icondef") {
        clearEmoticonsMap();

        while (!m_reader.atEnd()) {
          m_reader.readNext();

          if (m_reader.isEndElement())
            break;

          if (m_reader.isStartElement()) {
            if (m_reader.name() == "icon")
              readIcon();
            else
              readUnknownElement();
          }
        }

      }
      else
        m_reader.raiseError("bad icondef file");
    }
  }

  return !m_reader.error();
}


void XmppEmoticons::readIcon()
{
  QString emo;
  QStringList sl;

  while (!m_reader.atEnd()) {
    m_reader.readNext();

    if (m_reader.isEndElement())
      break;

    if (m_reader.isStartElement()) {
      if (m_reader.name() == "text") {
        QString text = m_reader.readElementText();
        if (!text.isEmpty())
          sl << text;
      }
      else if (m_reader.name() == "object") {
        QString mime = m_reader.attributes().value("mime").toString();
        QString object = m_reader.readElementText();
        if (!object.isEmpty() && m_mime.contains(mime))
          emo = object;
      }
      else
        readUnknownElement();
    }
  }

  if (!emo.isEmpty()) {
    emo = themePath() + '/' + emo;
    if (QFile::exists(emo)) {
      addEmoticonIndex(emo, sl);
      addEmoticonsMap(emo, sl);
    }
  }
}


void XmppEmoticons::readUnknownElement()
{
  while (!m_reader.atEnd()) {
    m_reader.readNext();

    if (m_reader.isEndElement())
      break;

    if (m_reader.isStartElement())
      readUnknownElement();
  }
}
