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

#include "ChatCore.h"
#include "ChatPlugins.h"
#include "ChatSettings.h"
#include "plugins/ChatApi.h"
#include "plugins/ChatPlugin.h"
#include "plugins/CoreApi.h"
#include "sglobal.h"

ChatPlugins::ChatPlugins(QObject *parent)
  : Plugins(parent)
{
  m_type = LS("chat");
}


/*!
 * Загрузка плагинов.
 */
void ChatPlugins::init()
{
  ChatSettings *settings = ChatCore::settings();

  for (int i = 0; i < m_sorted.size(); ++i) {
    PluginItem *item = m_plugins.value(m_sorted.at(i));
    ChatApi *api = qobject_cast<ChatApi *>(item->plugin());

    if (!api)
      continue;

    const QString key = LS("Plugins/") + item->id();
    settings->setLocalDefault(key, item->header().value(LS("Enabled")));
    if (settings->value(key) == false)
      continue;

    ChatPlugin *plugin = api->create();
    if (!plugin) {
      settings->setValue(key, false);
      continue;
    }

    item->setLoaded(true);
    m_chatPlugins.append(plugin);
  }
}