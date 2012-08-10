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

#ifndef RESTAPIPLUGIN_H_
#define RESTAPIPLUGIN_H_

#include "CoreApi.h"
#include "NodeApi.h"

class RestApiPlugin : public QObject, CoreApi, NodeApi
{
  Q_OBJECT
  Q_INTERFACES(CoreApi NodeApi)

public:
  QVariantMap header() const
  {
    QVariantMap out = CoreApi::header();
    out["Id"]       = "RestApi";
    out["Name"]     = "REST API";
    out["Version"]  = "0.1.0";
    out["Type"]     = "server";
    out["Site"]     = "http://wiki.schat.me/Plugin/RestApi";
    out["Desc"]     = "REST API";
    out["Required"] = "1.99.38";

    return out;
  }

  NodePlugin *create();
};

#endif /* RESTAPIPLUGIN_H_ */
