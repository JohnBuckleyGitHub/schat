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
#include <QCoreApplication>
#include <qplugin.h>

#include "DebugClientPlugin.h"
#include "FileLocations.h"
#include "net/SimpleClient.h"
#include "Settings.h"

DebugClientPlugin::DebugClientPlugin()
  : m_settings(0)
  , m_client(0)
{
}

QString DebugClientPlugin::name() const
{
  return "Debug Client";
}


void DebugClientPlugin::setClient(SimpleClient *client)
{
  qDebug() << "             " << m_client << m_settings;
  m_client = client;
  connect(m_client, SIGNAL(connected()), SLOT(connected()));
}


void DebugClientPlugin::setSettings(Settings *settings)
{
  qDebug() << "             " << m_client << m_settings;
  m_settings = settings;
}


void DebugClientPlugin::connected()
{
//  FileLocations l;
  qDebug() << "                     connected()" << QCoreApplication::applicationDirPath() << m_settings->locations()->path(FileLocations::ConfigFile);
}

Q_EXPORT_PLUGIN2(DebugClient, DebugClientPlugin);
