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

#include "BotPlugins.h"
#include "client/ClientHelper.h"
#include "client/SimpleClient.h"
#include "FileLocations.h"

BotPlugins::BotPlugins(QObject *parent)
  : Plugins(parent)
{
  m_client = new SimpleClient(new User("Bot"), 0, this);
  m_helper = new ClientHelper(m_client);
  m_locations = new FileLocations(this);

  m_providers.insert("BotCore", 0);
}


void BotPlugins::init()
{
  m_client->openUrl("schat://192.168.1.33");
}
