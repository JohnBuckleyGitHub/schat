/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>

#include "adium_emoticons.h"

AdiumEmoticons::AdiumEmoticons(QObject *parent)
  : EmoticonsProvider(parent)
{
}


bool AdiumEmoticons::loadTheme(const QString &path)
{
  EmoticonsProvider::loadTheme(path);

  QFile fp(path);

  if (!fp.exists()) {
    qWarning() << path << "doesn't exist!";
    return false;
  }

  if (!fp.open(QIODevice::ReadOnly)) {
    qWarning() << fp.fileName() << "can't open ReadOnly!";
    return false;
  }

  QString error;
  int eli, eco;
  if (!m_themeXml.setContent(&fp, &error, &eli, &eco)) {
    qWarning() << fp.fileName() << "can't copy to xml!";
    qWarning() << error << "line:" << eli << "column:" << eco;
    fp.close();
    return false;
  }

  fp.close();

  QDomElement fce = m_themeXml.firstChildElement("plist").firstChildElement(
      "dict").firstChildElement("dict");

  if (fce.isNull()) {
    return false;
  }

  QDomNodeList nl = fce.childNodes();

  clearEmoticonsMap();
  QString name;
  for (uint i = 0; i < nl.length(); i++) {
    QDomElement de = nl.item(i).toElement();

    if (!de.isNull() && de.tagName() == "key") {
      name = themePath() + '/' + de.text();
      continue;
    }
    else if (!de.isNull() && de.tagName() == "dict") {
      QDomElement arr = de.firstChildElement("array");
      QDomNodeList snl = arr.childNodes();
      QStringList sl;

      for (uint k = 0; k < snl.length(); k++) {
        QDomElement sde = snl.item(k).toElement();

        if (!sde.isNull() && sde.tagName() == "string") {
          sl << sde.text();
        }
      }
      if (QFile::exists(name)) {
        addEmoticonIndex(name, sl);
        addEmoticonsMap(name, sl);
        name.clear();
      }
    }
  }

  return true;
}
