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

#ifndef NODECONSOLEPLUGIN_H_
#define NODECONSOLEPLUGIN_H_

#include "CoreApi.h"
#include "NodeApi.h"

class NodeConsolePlugin : public QObject, CoreApi, NodeApi
{
  Q_OBJECT
  Q_INTERFACES(CoreApi NodeApi)

# if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "me.schat.server.NodeConsole" FILE "NodeConsole.json")
# endif

public:
  QVariantMap header() const
  {
    QVariantMap out        = CoreApi::header();
    out[CORE_API_ID]       = "NodeConsole";
    out[CORE_API_NAME]     = "Node Console";
    out[CORE_API_VERSION]  = "0.1.1";
    out[CORE_API_TYPE]     = "server";
    out[CORE_API_SITE]     = "http://wiki.schat.me/Plugin/NodeConsole";
    out[CORE_API_DESC]     = "Node Console";

    return out;
  }

  NodePlugin *create();
};

#endif /* NODECONSOLEPLUGIN_H_ */
