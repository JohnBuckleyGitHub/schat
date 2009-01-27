/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include "kde_emoticons.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtGui/QImageReader>
#include <QtGui/QTextDocument>
#include <QDebug>

KdeEmoticons::KdeEmoticons(QObject *parent)
  : EmoticonsProvider(parent)
{
}


bool KdeEmoticons::loadTheme(const QString &path)
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

  QDomElement fce = m_themeXml.firstChildElement("messaging-emoticon-map");

  if (fce.isNull())
    return false;

  QDomNodeList nl = fce.childNodes();

  clearEmoticonsMap();

  for (uint i = 0; i < nl.length(); i++) {
    QDomElement de = nl.item(i).toElement();

    if (!de.isNull() && de.tagName() == "emoticon") {
      QDomNodeList snl = de.childNodes();
      QStringList sl;

      for (uint k = 0; k < snl.length(); k++) {
        QDomElement sde = snl.item(k).toElement();

        if (!sde.isNull() && sde.tagName() == "string") {
          sl << sde.text();
        }
      }

      QString emo = themePath() + '/' + de.attribute("file");

      if (!QFile::exists(emo)) {
        QList<QByteArray> ext = QImageReader::supportedImageFormats();

        for (int j = 0; j < ext.size(); ++j) {
          emo = themePath() + '/' + de.attribute("file") + '.' + ext.at(j);
          if (QFile::exists(emo)) {
            break;
          }
        }

        if (!QFile::exists(emo)) {
          continue;
        }
      }

      addEmoticonIndex(emo, sl);
      addEmoticonsMap(emo, sl);
    }
  }

  return true;
}
