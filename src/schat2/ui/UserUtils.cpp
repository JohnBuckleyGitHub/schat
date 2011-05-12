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

#include "ui/UserUtils.h"
#include "User.h"

QIcon UserUtils::icon(User *user)
{
  QString file = ":/images/user";
  int gender = user->gender();
  int color  = user->color();

  if (gender == User::Unknown)
    file += "-unknown";
  else if (color == User::Black)
    file += "-black";
  else if (color == User::Gray)
    file += "-gray";
  else if (color == User::Green)
    file += "-green";
  else if (color == User::Red)
    file += "-red";
  else if (color == User::White)
    file += "-white";
  else if (color == User::Yellow)
    file += "-yellow";
  else if (color == User::Medical)
    file += "-medical";
  else if (color == User::Nude)
    file += "-nude";
  else if (color == User::Thief)
    file += "-thief";

  if (gender == User::Female)
    file += "-female";

  return QIcon(file + ".png");

}
