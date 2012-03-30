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

#ifndef OSINFO_H_
#define OSINFO_H_

#include <QVariant>

#include "schat.h"

class SCHAT_EXPORT OsInfo
{
public:
  ///< Тип операционной системы.
  enum OsType {
    Unknown = 0,  ///< Неизвестная операционная система.
    Windows = 87, ///< 'W' Windows.
    MacOSX  = 77, ///< 'M' Mac OS X.
    Linux   = 76  ///< 'L' Linux.
  };

  OsInfo();
  inline const QVariantMap& json() const { return m_json; }
  static int type();

private:
  QVariantMap m_json;
};

#endif /* OSINFO_H_ */