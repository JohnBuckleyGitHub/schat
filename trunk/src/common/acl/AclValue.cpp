/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include "acl/AclValue.h"
#include "acl/Acl.h"

AclValue::AclValue()
  : m_mask(0)
{
}


AclValue::AclValue(const QByteArray &mask)
  : m_mask(toInt(mask))
{
}


AclValue::AclValue(int mask)
  : m_mask(mask)
{
}


QByteArray AclValue::toByteArray() const
{
  return toByteArray(m_mask);
}


/*!
 * Преобразование строкового представления прав доступа в число.
 */
int AclValue::toInt(const QByteArray &mask)
{
  if (mask.size() < 3)
    return 0;

  int out = 0;

  if (mask.at(0) == 'r')
    out |= Acl::Read;
  else if (mask.at(0) == 'R')
    out |= Acl::Read | Acl::SpecialRead;
  else if (mask.at(1) == 'w')
    out |= Acl::Write;
  else if (mask.at(1) == 'W')
    out |= Acl::Write | Acl::SpecialWrite;
  else if (mask.at(2) == 'x')
    out |= Acl::Edit;
  else if (mask.at(2) == 'X')
    out |= Acl::Edit | Acl::SpecialEdit;

  return out;
}


/*!
 * Преобразование числового представления правд доступа в строку.
 */
QByteArray AclValue::toByteArray(int mask)
{
  QByteArray out("---");

  if (mask & Acl::Read)
    out[0] = mask & Acl::SpecialRead  ? 'R' : 'r';

  if (mask & Acl::Write)
    out[1] = mask & Acl::SpecialWrite ? 'W' : 'w';

  if (mask & Acl::Edit)
    out[2] = mask & Acl::SpecialEdit  ? 'X' : 'x';

  return out;
}
