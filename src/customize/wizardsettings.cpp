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

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QProcess>

#include "version.h"
#include "wizardsettings.h"

/*!
 * \brief Конструктор класса DaemonSettings.
 */
WizardSettings::WizardSettings(const QString &filename, QObject *parent)
  : AbstractSettings(filename, parent)
{
  m_dist = checkDist();
  if (!m_dist)
    m_rootPath = QCoreApplication::applicationDirPath();
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
  int index = environment.indexOf(QRegExp(env + "=.*"));
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
  setString("MakensisFile",      pf);
  setString("Network",           "SimpleNet.xml");
  setString("Emoticons",         "Kolobok");
  setString("MirrorUrl",         "http://impomezia.com/mirror.xml");
  setString("PfxFile",           "");
  setString("PfxPassword",       "");
  setString("Translation",       "auto");

  if (m_dist)
    setString("BaseUrl",         "http://schat.googlecode.com/files");
  else
    setString("BaseUrl",         "");

  setBool("OverrideNetwork",     false);
  setBool("OverrideEmoticons",   false);
  setBool("OverrideMirror",      false);
  setBool("NsisDesktop",         false);
  setBool("NsisQuickLaunch",     true);
  setBool("NsisAllPrograms",     true);
  setBool("NsisAutostart",       true);
  setBool("NsisAutostartDaemon", false);
  setBool("AutoDownloadUpdates", false);
  setBool("Comparable",          false);

  AbstractSettings::read();

  if (m_dist || !getBool("Save"))
    setVersions();

  m_ro << "Comparable";
}


QString WizardSettings::version()
{
  QString version = QCoreApplication::applicationVersion();
  return version.left(version.indexOf(' '));
}


/*!
 * Определяет режим работы программы, работа из дерева исходников, для формирования
 * официального дистрибутива или работа в обычном режиме, для формирования
 * пользовательского дистрибутива.
 *
 * \return Возращает \a true если программа запущена из дерева исходников.
 */
bool WizardSettings::checkDist()
{
  QDir dir(QCoreApplication::applicationDirPath() + "/../../");
  QString path = dir.absolutePath();

  if (!QFile::exists(path + "/schat.pro"))
    return false;

  if (!QFile::exists(path + "/src/common/version.h"))
    return false;

  if (!QFile::exists(path + "/src/schat/schat.pro"))
    return false;

  if (!QFile::exists(path + "/os/win32/setup.nsi"))
    return false;

  m_rootPath = path;
  return true;
}


void WizardSettings::setVersions()
{
  setString("Version",      version());
  setString("Suffix",       "");
  setBool("Mirror",         m_dist);
  setBool("MirrorCore",     true);
  setBool("MirrorQt",       !m_dist);
  setBool("OverrideLevels", false);
  setBool("Save",           false);
  setInt("LevelCore",       UpdateLevelCore);
  setInt("LevelQt",         UpdateLevelQt);
}
