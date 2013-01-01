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

#include "cores/Core.h"
#include "NodePlugins.h"
#include "plugins/NodeApi.h"
#include "plugins/NodePlugin.h"
#include "Settings.h"
#include "Storage.h"
#include "sglobal.h"

NodePlugins *NodePlugins::m_self = 0;

NodePlugins::NodePlugins(QObject *parent)
  : Plugins(parent)
{
  m_self = this;
  m_type = LS("server");
}


void NodePlugins::init()
{
  for (int i = 0; i < m_sorted.size(); ++i) {
    NodeApi *api = qobject_cast<NodeApi *>(m_plugins.value(m_sorted.at(i))->plugin());

    if (!api)
      continue;

    NodePlugin *plugin = api->create();
    if (!plugin)
      continue;

    m_nodePlugins.append(plugin);
  }
}
