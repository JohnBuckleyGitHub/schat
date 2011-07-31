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

#include "ChatCore.h"
#include "ChatPlugins.h"
#include "plugins/ChatApi.h"
#include "plugins/ChatPlugin.h"
#include "plugins/CoreApi.h"

ChatPlugins::ChatPlugins(QObject *parent)
  : Plugins(parent)
  , m_core(ChatCore::i())
{
}


void ChatPlugins::init()
{
  for (int i = 0; i < m_plugins.size(); ++i) {
    ChatApi *api = qobject_cast<ChatApi *>(m_plugins.at(i));
    if (api) {
      m_chatPlugins.append(api->init(m_core));
      qDebug() << api;
    }
  }
}
