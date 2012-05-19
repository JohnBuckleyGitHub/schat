/* IMPOMEZIA Simple Chat
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

#include <QtPlugin>
#include <QDesktopServices>
#include <QWebSettings>

#include "ChatCore.h"
#include "ChatNotify.h"
#include "ChatSettings.h"
#include "YouTubeFilter.h"
#include "YouTubeChatView.h"
#include "YouTubePlugin.h"
#include "YouTubePlugin_p.h"
#include "net/SimpleID.h"
#include "sglobal.h"

YouTubePluginImpl::YouTubePluginImpl(QObject *parent)
  : ChatPlugin(parent)
{
  QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);

  TokenFilter::add(LS("channel"), new YouTubeFilter());
  new YouTubeChatView(this);
}

ChatPlugin *YouTubePlugin::create()
{
  if (!ChatCore::config().contains(LS("EXTENSIONS")))
    return 0;

  m_plugin = new YouTubePluginImpl(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(YouTube, YouTubePlugin);
