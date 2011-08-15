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

#include <QDebug>

#include "cores/Core.h"
#include "NodePlugins.h"
#include "plugins/NodeKernelApi.h"
#include "Settings.h"
#include "Storage.h"

NodePlugins::NodePlugins(QObject *parent)
  : Plugins(parent)
  , m_core(0)
{
  m_kernelId = Storage::i()->settings()->value(QLatin1String("Kernel")).toString();
  if (!m_kernelId.isEmpty())
    addProvider(m_kernelId);
}


Core *NodePlugins::kernel()
{
  if (!m_core)
    m_core = new Core(this);

  return m_core;
}


void NodePlugins::init()
{
  if (!m_kernelId.isEmpty() && m_providers.value(m_kernelId)) {
    NodeKernelApi *api = qobject_cast<NodeKernelApi *>(m_providers.value(m_kernelId)->plugin());
    m_core = api->init();
  }

  kernel();
}
