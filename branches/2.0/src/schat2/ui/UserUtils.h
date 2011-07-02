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

#ifndef USERUTILS_H_
#define USERUTILS_H_

#include <QIcon>

#include "schat2.h"

class UserUtils
{
public:
  UserUtils();
  static int color(const QString &color);
  static QIcon icon(ChatUser user, bool status = true);
  static QString overlay(int status);
  static QString statusTitle(int status);
  static QString toolTip(ChatUser user);

private:
  static QStringList m_colors;
};

#endif /* USERUTILS_H_ */
