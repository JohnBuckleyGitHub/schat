/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 * Base class Emoticons
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

#ifndef EMOTICONS_H_
#define EMOTICONS_H_

#include <QObject>
#include <QHash>

#include "emoticonstheme.h"

class EmoticonsPrivate;

/*!
 * \brief This class can be used to retrieve, install, create emoticons theme.
 * For example if you want to get the current emoticon theme
 * \code
 * Emoticons ke;
 * EmoticonsTheme et = ke.theme();
 * //do whatever you want with the theme
 * \endcode
 * it can also be used to set the emoticon theme and the parse mode in the config file
 * \author Carlo Segato (brandon.ml@gmail.com)
 */
class Emoticons : public QObject
{
  Q_OBJECT

public:
  Emoticons(QObject *parent = 0);
  ~Emoticons();
  EmoticonsTheme theme();
  EmoticonsTheme theme(const QString &name);
  static bool strictParse();
  static QString currentThemeName();
  static QStringList themeList();
  static void setStrictParse(bool strict = true);
  static void setTheme(const EmoticonsTheme &theme);
  static void setTheme(const QString &theme);

private:
  EmoticonsPrivate * const d;
};

#endif /*EMOTICONS_H_*/
