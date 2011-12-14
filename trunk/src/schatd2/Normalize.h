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

#ifndef NORMALIZE_H_
#define NORMALIZE_H_

#include <QHash>

#include "schat.h"

class SCHAT_EXPORT Normalize
{
  Normalize() {}

public:
  static QByteArray toId(const QString &text);
  static QString toString(const QString &text);
  static void init();

private:
  static QHash<QChar, QChar> m_map; ///< Карта замены символов.
};

#endif /* NORMALIZE_H_ */
