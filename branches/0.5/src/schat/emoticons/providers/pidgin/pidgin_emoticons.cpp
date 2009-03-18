/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 * Base class PidginEmoticons
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
#include <QTextStream>

#include "pidgin_emoticons.h"

/*!
 * Конструктор класса PidginEmoticons.
 */
PidginEmoticons::PidginEmoticons(QObject *parent)
  : EmoticonsProvider(parent)
{
}


bool PidginEmoticons::loadTheme(const QString &path)
{
  EmoticonsProvider::loadTheme(path);

  QFile fp(path);

  if (!fp.exists())
    return false;

  if (!fp.open(QIODevice::ReadOnly))
    return false;

  QTextStream str(&fp);
  bool start = false;
  m_text.clear();
  while (!str.atEnd()) {
    QString line = str.readLine();
    m_text << line;

    if (line.startsWith('#') || line.isEmpty()) {
      continue;
    }

    QRegExp re("^\\[(.*)\\]$");
    int pos = re.indexIn(line.trimmed());
    if (pos > -1) {
      if (!re.cap(1).compare("default", Qt::CaseInsensitive)) {
        start = true;
      }
      else {
        start = false;
      }
      continue;
    }

    if (!start) {
      continue;
    }

    QStringList splitted = line.split(QRegExp("\\s+"));
    QString emo;
    int i = 1;
    if (splitted.at(0) == "!") {
      i = 2;
      emo = themePath() + '/' + splitted.at(1);
    }
    else {
      emo = themePath() + '/' + splitted.at(0);
    }

    QStringList sl;
    for (; i < splitted.size(); ++i) {
      if (!splitted.at(i).isEmpty() && splitted.at(i) != " ") {
        sl << splitted.at(i);
      }
    }

    addEmoticonIndex(emo, sl);
    addEmoticonsMap(emo, sl);
  }

  fp.close();

  return true;
}
