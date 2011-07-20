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
#include <QTextDocument>

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


/*!
 * Иконка пользователя.
 *
 * \param user    Пользователь.
 * \param status  true если необходимо отрисовывать статус.
 * \param offline true если необходимо отрисовывать статус User::OfflineStatus.
 */
QIcon UserUtils::icon(ClientUser user, bool status, bool offline)
{
  QString file = QLatin1String(":/images/user");
  int gender = user->gender();
  int color  = user->color();

  if (gender == User::Unknown) {
    file += QLatin1String("-unknown");
  }
  else if (gender == User::Ghost) {
    file += QLatin1String("-ghost");
  }
  else if (gender == User::Bot) {
    file += QLatin1String("-bot");
  }
  else if (color != User::Default) {
    file += QLatin1String("-") + m_colors.at(color);
  }

  if (gender == User::Female)
    file += QLatin1String("-female");

  file += QLatin1String(".png");

  if (status) {
    if (offline && user->status() == User::OfflineStatus)
      return QIcon(QIcon(file).pixmap(16, 16, QIcon::Disabled));

    return ChatCore::icon(file, overlay(user->status()));
  }

  return QIcon(file);
}


QString UserUtils::overlay(int status)
{
  if (status == User::AwayStatus || status == User::AutoAwayStatus)
    return QLatin1String(":/images/away-small.png");
  else if (status == User::DnDStatus)
    return QLatin1String(":/images/dnd-small.png");
  else if (status == User::FreeForChatStatus)
    return QLatin1String(":/images/ffc-small.png");
  else
    return QLatin1String("");
}


/*!
 * Имя статуса.
 */
QString UserUtils::statusTitle(int status)
{
  switch (status) {
    case User::OfflineStatus:
      return QObject::tr("Offline");
      break;

    case User::AwayStatus:
    case User::AutoAwayStatus:
      return QObject::tr("Away");
      break;

    case User::DnDStatus:
      return QObject::tr("Do not disturb");
      break;

    case User::FreeForChatStatus:
      return QObject::tr("Free for Chat");
      break;

    default:
      return QObject::tr("Online");
      break;
  }
}


QString UserUtils::toolTip(ClientUser user)
{
  QString out = "<b>" + Qt::escape(user->nick()) + "</b><br />";
  out += Qt::escape(user->host()) + "<br />";
  out += Qt::escape(user->userAgent()) + "<br />";
  out += statusTitle(user->status()) + " " + Qt::escape(user->statusText());

  return out;
}
