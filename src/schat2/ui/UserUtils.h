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

#include "schat.h"
#include "User.h"

class SCHAT_CORE_EXPORT UserUtils
{
public:
  UserUtils();
  static ClientUser user();
  static ClientUser user(const QByteArray &id);
  static int color(const QString &color);
  static QByteArray userId();
  static QIcon icon(ClientUser user, bool status = true, bool offline = false);
  static QString overlay(int status);
  static QString statusTitle(int status);
  static QString toolTip(ClientUser user);

private:
  static QStringList m_colors;
};

#endif /* USERUTILS_H_ */
