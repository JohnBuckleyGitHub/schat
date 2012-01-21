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

#include <QtPlugin>

#include "FeedsImpl.h"
#include "RawFeedsCmd.h"
#include "RawFeedsPlugin.h"
#include "RawFeedsPlugin_p.h"
#include "RawFeedsChatView.h"

RawFeeds::RawFeeds(QObject *parent)
  : ChatPlugin(parent)
  , m_enabled(false)
{
  new Hooks::RawFeedsCmd(this);
  new Hooks::FeedsImpl(this);
  new RawFeedsChatView(this);
}


ChatPlugin *RawFeedsPlugin::create()
{
  m_plugin = new RawFeeds(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(RawFeeds, RawFeedsPlugin);
