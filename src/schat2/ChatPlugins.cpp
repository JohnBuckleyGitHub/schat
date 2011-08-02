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

#include <debugstream.h>

#include "ChatCore.h"
#include "ChatPlugins.h"
#include "ChatSettings.h"
#include "plugins/ChatApi.h"
#include "plugins/ChatPlugin.h"
#include "plugins/CoreApi.h"

ChatPlugins::ChatPlugins(QObject *parent)
  : Plugins(parent)
  , m_core(ChatCore::i())
  , m_settings(ChatCore::i()->settings())
{
  addProvider("History");
}


void ChatPlugins::hook(const HookData &data)
{
  if (!m_hooks.contains(data.type()))
    return;

  QList<ChatPlugin *> list = m_hooks.value(data.type());
  for (int i = 0; i < list.size(); ++i) {
    list.at(i)->hook(data);
  }
}


/*!
 * Загрузка плагинов.
 */
void ChatPlugins::init()
{
  for (int i = 0; i < m_plugins.size(); ++i) {
    ChatApi *api = qobject_cast<ChatApi *>(m_plugins.at(i));

    if (!api)
      continue;

    ChatPlugin *plugin = api->init(m_core);
    m_chatPlugins.append(plugin);
    connect(m_core, SIGNAL(notify(int, const QVariant &)), plugin, SLOT(notify(int, const QVariant &)));
    connect(m_settings, SIGNAL(changed(const QString &, const QVariant &)), plugin, SLOT(settingsChanged(const QString &, const QVariant &)));

    QList<HookData::Type> hooks = plugin->hooks();
    if (hooks.isEmpty())
      continue;

    foreach (HookData::Type hook, hooks) {
      m_hooks[hook].append(plugin);
    }
  }
}
