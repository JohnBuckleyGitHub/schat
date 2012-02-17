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

#include "Channel.h"
#include "ChatSettings.h"
#include "sglobal.h"

ChatSettings::ChatSettings(const QString &fileName, QObject *parent)
  : Settings(fileName, parent)
{
  setDefault(LS("AutoConnect"),           true);
  setDefault(LS("AutoJoin"),              true);
  setDefault(LS("ChannelUserCount"),      false);
  setDefault(LS("DeveloperExtras"),       false);
  setDefault(LS("Height"),                420);
  setDefault(LS("HideIgnore"),            true);
  setDefault(LS("Maximized"),             false);
  setDefault(LS("Networks"),              QStringList());
  setDefault(LS("ShowSeconds"),           false);
  setDefault(LS("ShowServiceMessages"),   false);
  setDefault(LS("Translation"),           LS("auto"));
  setDefault(LS("Width"),                 666);
  setDefault(LS("WindowsAero"),           true);

  setDefault(LS("Labs/CookieAuth"),       true);
  setDefault(LS("Labs/DisableUI"),        false);
  setDefault(LS("Labs/StaticTrayAlerts"), false);

  setDefault(LS("Profile/Gender"),        0);
  setDefault(LS("Profile/Nick"),          Channel::defaultName());
  setDefault(LS("Profile/Status"),        1);
}
