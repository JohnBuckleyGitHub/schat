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

#include <QPainter>

#include "ChatCore.h"
#include "ui/UserUtils.h"
#include "User.h"

QStringList UserUtils::m_colors;

UserUtils::UserUtils()
{
  m_colors += "default";
  m_colors += "black";
  m_colors += "gray";
  m_colors += "green";
  m_colors += "red";
  m_colors += "white";
  m_colors += "yellow";
  m_colors += "medical";
  m_colors += "nude";
  m_colors += "thief";
}


int UserUtils::color(const QString &color)
{
  int index = m_colors.indexOf(color);
  if (index == -1)
    return User::Default;

  return index;
}



QIcon UserUtils::icon(ChatUser user, bool status)
{
  QString file = ":/images/user";
  int gender = user->gender();
  int color  = user->color();

  if (gender == User::Unknown) {
    file += "-unknown";
  }
  else if (gender == User::Ghost) {
    file += "-ghost";
  }
  else if (color != User::Default) {
    file += "-" + m_colors.at(color);
  }

  if (gender == User::Female)
    file += "-female";

  file += ".png";

  if (status) {
    if (user->status() == User::AwayStatus || user->status() == User::AutoAwayStatus)
      return ChatCore::icon(file, QLatin1String(":/images/away-small.png"));
    else if (user->status() == User::DnDStatus)
      return ChatCore::icon(file, QLatin1String(":/images/dnd-small.png"));
  }

  return QIcon(file);
}


QString UserUtils::toolTip(ChatUser user)
{
  QString out = "<b>" + user->nick() + "</b><br />";
  out += user->host() + "<br />";
  out += user->userAgent() + "<br />";

  switch (user->status()) {
    case User::OfflineStatus:
      out += "Offline";
      break;

    case User::AwayStatus:
    case User::AutoAwayStatus:
      out += "Away";
      break;

    case User::DnDStatus:
      out += "DnD";
      break;

    default:
      out += "Online";
      break;
  }
  return out;
}
