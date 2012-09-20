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

#ifndef GEOIPPLUGIN_H_
#define GEOIPPLUGIN_H_

#include "CoreApi.h"
#include "NodeApi.h"

class GeoIPPlugin : public QObject, CoreApi, NodeApi
{
  Q_OBJECT
  Q_INTERFACES(CoreApi NodeApi)

# if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "me.schat.server.GeoIP" FILE "GeoIP.json")
# endif

public:
  QVariantMap header() const
  {
    QVariantMap out = CoreApi::header();
    out["Id"]       = "GeoIP";
    out["Name"]     = "GeoIP";
    out["Version"]  = "0.1.1";
    out["Type"]     = "server";
    out["Site"]     = "http://wiki.schat.me/Plugin/GeoIP";
    out["Desc"]     = "Geo IP Support";

    return out;
  }

  NodePlugin *create();
};

#endif /* GEOIPPLUGIN_H_ */
