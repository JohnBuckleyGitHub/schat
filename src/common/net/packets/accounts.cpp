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

#include <QDebug>

#include "net/packets/accounts.h"
#include "User.h"

QString RegReply::filter(const QString &name)
{
  QString out = name.simplified().toLower();
  out.remove(' ');
  out.remove('@');

  return out.left(User::MaxNickLength);
}


QString LoginReply::filter(const QString &name, const QString &serverName)
{
  QString out = name.simplified().toLower();
  out.remove(' ');

  int index = out.indexOf('@');
  if (index == -1) {
    out = out.left(User::MaxNickLength);
  }
  else if (index == 0) {
    return QString();
  }
  else {
    if (out.mid(index + 1) != serverName)
      return QString();

    out = out.left(index).left(User::MaxNickLength);
  }

  out += "@" + serverName;
  return out;
}
