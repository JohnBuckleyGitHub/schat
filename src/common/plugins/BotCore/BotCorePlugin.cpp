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
#include <QTimer>
#include <QSettings>

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
  m_settings.insert("Url", "schat://schat.me");
  m_settings.insert("Nick", "Bot");
  m_settings.insert("Gender", 152);
  m_settings.insert("Status", "1;");

  readSettings();
  QTimer::singleShot(0, this, SLOT(start()));
}


void BotCore::start()
{
  m_client->openUrl(m_settings.value("Url").toString());
}


void BotCore::readSettings()
{
  QSettings settings(m_locations->path(FileLocations::ConfigFile), QSettings::IniFormat);
  settings.setIniCodec("UTF-8");
  settings.beginGroup("BotCore");

  QMutableHashIterator<QString, QVariant> i(m_settings);
  while (i.hasNext()) {
    i.next();
    m_settings[i.key()] = settings.value(i.key(), i.value());
  }

  m_client->setNick(m_settings.value("Nick").toString());

//  ClientUser user = m_client->user();
//  user->gender().setRaw(m_settings.value("Gender").toInt());
//  user->setStatus(m_settings.value("Status").toString());
}


QObject *BotCorePlugin::init(ClientHelper *helper, FileLocations *locations)
{
  m_core = new BotCore(helper, locations);
  return m_core;
}

Q_EXPORT_PLUGIN2(BotCore, BotCorePlugin);
