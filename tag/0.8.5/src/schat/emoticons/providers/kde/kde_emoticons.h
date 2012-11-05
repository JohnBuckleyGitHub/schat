/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#ifndef KDE_EMOTICONS_H_
#define KDE_EMOTICONS_H_

#include <QObject>
#include <QXmlStreamReader>

#include <emoticons/emoticonsprovider.h>

/*!
 * \brief Класс для чтения пакета смайликов KDE формата.
 */
class KdeEmoticons : public EmoticonsProvider
{
  Q_OBJECT

public:
  KdeEmoticons(QObject *parent);
  bool loadTheme(const QString &path);

private:
  void readMap();
  void readUnknownElement();

  QXmlStreamReader m_reader;
};

#endif /*KDE_EMOTICONS_H_*/
