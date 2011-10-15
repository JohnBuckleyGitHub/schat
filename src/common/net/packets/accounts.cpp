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

#include "net/packets/accounts.h"

/*!
 * Сериализация в JSON.
 */
QVariant RegReply::json() const
{
  QVariantMap map;

  if (m_result == OK) {
    map["result"] = "ok";

    if (!m_name.isEmpty())
      map["name"] = m_name;
  }
  else {
    map["result"] = "error";
    map["error"] = m_error;
//    map["error-desc"] = error(m_error);
  }

  return map;
}


QString RegReply::filter(const QString &name)
{
  QString out = name.simplified().toLower();
  out.remove(' ');
  out.remove('@');

  return out.left(64);
}
