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
 * Получение значения системной переменной окружения.
 *
 * \param env Имя переменной.
 * \return    Значение переменной или пустая строка.
 */
QString WizardSettings::envValue(const QString &env)
{
  QStringList environment = QProcess::systemEnvironment();
  int index = environment.indexOf(QRegExp(env + ".*"));
  if (index != -1) {
    QStringList list = environment.at(index).split("=");
    if (list.size() == 2)
      return list.at(1);
    else
      return "";
  }
  else
    return "";
}


/*!
 * Чтение настроек
 */
void WizardSettings::read()
{
  setVersions();

  QString pf = envValue("ProgramFiles");
  if (!pf.isEmpty()) {
    pf += "/NSIS/makensis.exe";
    pf = QDir::toNativeSeparators(pf);
  }
  else
    pf = "makensis.exe";
  setString("MakensisFile", pf);

  AbstractSettings::read();

  if (!getBool("Save"))
    setVersions();
}


QString WizardSettings::version()
{
  QString version = QCoreApplication::applicationVersion();
  return version.left(version.indexOf(' '));
}


void WizardSettings::setVersions()
{
  setString("Version",      version());
  setString("Suffix",       "");
  setBool("Mirror",         false);
  setBool("MirrorCore",     true);
  setBool("MirrorQt",       true);
  setBool("OverrideLevels", false);
  setBool("Save",           false);
  setInt("LevelCore",       UpdateLevelCore);
  setInt("LevelQt",         UpdateLevelQt);
}
