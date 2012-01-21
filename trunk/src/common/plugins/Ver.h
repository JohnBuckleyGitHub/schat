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

#ifndef VER_H_
#define VER_H_

#include <QString>

#include "schat.h"

class SCHAT_EXPORT Ver
{
public:
  Ver();
  Ver(const QString &version);
  inline int major() const { return m_major; }
  inline int minor() const { return m_minor; }
  inline int patch() const { return m_patch; }
  QString toString() const;
  void setVersion(const QString &version);
private:
  int m_major;
  int m_minor;
  int m_patch;
};

#endif /* VER_H_ */
