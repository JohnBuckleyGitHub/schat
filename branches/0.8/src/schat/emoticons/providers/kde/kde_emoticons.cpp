/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
 *
 * Base class KdeEmoticons
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

#include "kde_emoticons.h"

#include <QFile>
#include <QFileInfo>
#include <QImageReader>

/*!
 * Конструктор класса KdeEmoticons.
 */
KdeEmoticons::KdeEmoticons(QObject *parent)
  : EmoticonsProvider(parent)
{
}


bool KdeEmoticons::loadTheme(const QString &path)
{
  EmoticonsProvider::loadTheme(path);

  QFile file(path);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return false;

  m_reader.setDevice(&file);

  while (!m_reader.atEnd()) {
    m_reader.readNext();

    if (m_reader.isStartElement()) {
      if (m_reader.name() == "messaging-emoticon-map") {
        clearEmoticonsMap();
        readMap();
      }
      else
        m_reader.raiseError("bad messaging-emoticon-map file");
    }
  }

  return !m_reader.error();
}


void KdeEmoticons::readMap()
{
  QList<QByteArray> ext = QImageReader::supportedImageFormats();

  while (!m_reader.atEnd()) {
    m_reader.readNext();

    if (m_reader.isEndElement())
      break;

    if (m_reader.isStartElement()) {
      if (m_reader.name() == "emoticon") {
        QString file = m_reader.attributes().value("file").toString();

        if (!file.isEmpty()) {
          QStringList sl;

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
            }
          }

          QString emo;
          QString base = themePath() + '/' + file;

          if (!QFile::exists(base)) {
            for (int j = 0; j < ext.size(); ++j) {
              emo = base + '.' + ext.at(j);
              if (QFile::exists(emo)) {
                break;
              }
            }
          }
          else
            emo = base;

          if (QFile::exists(emo)) {
            addEmoticonIndex(emo, sl);
            addEmoticonsMap(emo, sl);
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


void KdeEmoticons::readUnknownElement()
{
  while (!m_reader.atEnd()) {
    m_reader.readNext();

    if (m_reader.isEndElement())
      break;

    if (m_reader.isStartElement())
      readUnknownElement();
  }
}
