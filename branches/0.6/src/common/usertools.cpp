/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include <QStringList>

#include "usertools.h"

/*!
 * Проверка корректности ника.
 */
bool UserTools::isValidNick(const QString &nick)
{
  if (nick.isEmpty())
    return false;

  if (nick.size() > MaxNickSize)
    return false;

  if (nick.size() < MinNickSize)
    return false;

  if (nick.startsWith("#"))
    return false;

  if (nick == "..")
    return false;

  return true;
}


/*!
 * Проверка корректности агента пользователя.
 */
bool UserTools::isValidUserAgent(const QString &agent)
{
  if (agent.isEmpty())
    return false;

  if (agent.size() > MaxUserAgentSize)
    return false;

  if (agent.size() < MinUserAgentSize)
    return false;

  QStringList list = agent.split("/");
  if (list.size() == 2)
    return true;
  else
    return false;
}
