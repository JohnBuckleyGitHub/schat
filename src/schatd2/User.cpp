/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include "DateTime.h"
#include "User.h"
#include "sglobal.h"

Birthday::Birthday(quint8 day, quint8 month, quint16 year)
  : day(day)
  , month(month)
  , year(year)
{
}


User::User()
  : saved(true)
  , date(0)
  , channel(0)
{
}


/*!
 * Установка поля профиля.
 */
bool User::set(const QString &key, const QVariant &value)
{
  bool modified = false;

  if (key == LS("name"))
    return setString(name, value);
  else {
    modified = extra.value(key) != value;
    if (modified)
      extra[key] = value;
  }

  if (modified) {
    saved = false;
    date = DateTime::utc();
  }

  return modified;
}


QVariantMap User::toMap() const
{
  QVariantMap out;
  toMap(out);
  return out;
}


void User::toMap(QVariantMap &out) const
{
  QMapIterator<QString, QVariant> i(extra);
  while (i.hasNext()) {
    i.next();
    out[i.key()] = i.value();
  }

  if (!name.isEmpty())
    out[LS("name")] = name;
}


bool User::setString(QString &key, const QVariant &value)
{
  if (key == value)
    return false;

  date  = DateTime::utc();
  saved = false;
  key   = value.toString();
  return true;
}
