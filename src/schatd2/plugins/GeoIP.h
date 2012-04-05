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

#ifndef GEOIP_H_
#define GEOIP_H_

#include <QList>
#include <QVariantMap>

#include "schat.h"

class SCHAT_EXPORT GeoIP
{
public:
  GeoIP();
  virtual ~GeoIP();

  inline static void add(GeoIP *hook)    { m_hooks.append(hook); }
  inline static void remove(GeoIP *hook) { m_hooks.removeAll(hook); }

  static QVariantMap geo(const QString &ip);

protected:
  virtual void geo(const QString &ip, QVariantMap &out);

private:
  static QList<GeoIP*> m_hooks; ///< Хуки.
};

#endif /* GEOIP_H_ */
