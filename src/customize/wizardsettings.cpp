/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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

#include <QtCore>

#include "version.h"
#include "wizardsettings.h"

/*!
 * \brief Конструктор класса DaemonSettings.
 */
WizardSettings::WizardSettings(const QString &filename, QObject *parent)
  : AbstractSettings(filename, parent)
{
}


/*!
 * Чтение настроек
 */
void WizardSettings::read()
{
  setVersions();
  AbstractSettings::read();

  if (!getBool("Save"))
    setVersions();
}


void WizardSettings::setVersions()
{
  QString version = QCoreApplication::applicationVersion();
  version = version.left(version.indexOf(' '));
  setString("Version",      version);
  setString("Suffix",       "");
  setBool("Mirror",         false);
  setBool("MirrorCore",     true);
  setBool("MirrorQt",       true);
  setBool("OverrideLevels", false);
  setBool("Save",           false);
  setInt("LevelCore",       UpdateLevelCore);
  setInt("LevelQt",         UpdateLevelQt);
}
