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

#ifndef ACLVALUE_H_
#define ACLVALUE_H_

#include <QByteArray>

#include "schat.h"

class SCHAT_EXPORT AclValue
{
public:
  AclValue();
  AclValue(const QByteArray &mask);
  AclValue(int mask);
  QByteArray toByteArray() const;

  static int toInt(const QByteArray &mask);
  static QByteArray toByteArray(int mask);

private:
  int m_mask;
};

#endif /* ACLVALUE_H_ */
