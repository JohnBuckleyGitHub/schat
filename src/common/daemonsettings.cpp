/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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


#include "daemonsettings.h"

/*!
 * \brief Конструктор класса DaemonSettings.
 */
DaemonSettings::DaemonSettings(const QString &fileName, QObject *parent)
  : AbstractSettings(fileName, parent)
{
}


/*!
 * \brief Чтение настроек
 *
 * Сервер принципиально не поддерживает запись настроек.
 */
void DaemonSettings::read()
{
  setBool("ChannelLog",      false);
  setBool("PrivateLog",      false);
  setBool("LocalServer",     true);
  setBool("Network",         false);
  setBool("RootServer",      false);
  setBool("Motd",            true);
  setBool("Stats",           true);
  setInt("ListenPort",       7666);
  setInt("LogLevel",         0);
  setInt("Numeric",          0);
  setInt("MaxUsers",         0);
  setInt("MaxLinks",         10);
  setInt("MaxUsersPerIp",    0);
  setInt("MotdMaxSize",      2048);
  setInt("StatsInterval",    30);
  setString("ListenAddress", "0.0.0.0");
  setString("NetworkFile",   "network.xml");
  setString("Name",          "");
  setString("MotdFile",      "motd.html");
  setString("NormalizeFile", "normalize.xml");
  setString("StatsFile",     "stats.xml");
  setString("Translation",   "auto");
  setString("MigrateFile",   "migrate.json");
  mutableKeys();

  AbstractSettings::read();
}


void DaemonSettings::reload()
{
  mutableKeys();
  m_settings->sync();
  AbstractSettings::read();
}


/*!
 * Ключи настроек, которые могут быть изменены во время работы сервера.
 */
void DaemonSettings::mutableKeys()
{
  setInt("FloodDetectTime",     16);
  setInt("FloodLimit",          8);
  setInt("MaxRepeatedMsgs",     3);
  setInt("MuteTime",            60);
  setInt("JoinFloodDetectTime", 60);
  setInt("JoinFloodLimit",      3);
  setInt("JoinFloodBanTime",    120);
}
