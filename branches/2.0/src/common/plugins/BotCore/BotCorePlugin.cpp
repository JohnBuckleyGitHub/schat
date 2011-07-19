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

#include <QUrl>
#include <qplugin.h>

#include "BotCorePlugin.h"
#include "BotCorePlugin_p.h"
#include "client/ClientHelper.h"
#include "client/SimpleClient.h"
#include "FileLocations.h"

BotCore::BotCore(ClientHelper *helper, FileLocations *locations)
  : QObject(helper)
  , m_helper(helper)
  , m_locations(locations)
  , m_client(helper->client())
{
//  m_client->openUrl(QUrl("schat://192.168.1.33"));
}


QObject *BotCorePlugin::init(ClientHelper *helper, FileLocations *locations)
{
  m_core = new BotCore(helper, locations);
  return m_core;
}

Q_EXPORT_PLUGIN2(BotCore, BotCorePlugin);
