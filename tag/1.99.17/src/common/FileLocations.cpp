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
#include <QDir>
#include <QFileInfo>

#include "FileLocations.h"

FileLocations::FileLocations(QObject *parent)
  : QObject(parent)
{
  FileScheme scheme = Portable;
  QString appDirPath = QCoreApplication::applicationDirPath();
  QString baseName = QFileInfo(QCoreApplication::applicationFilePath()).baseName();

  m_paths.insert(AppDirPath, appDirPath);
  m_paths.insert(BaseName, baseName);

  #if defined(Q_WS_MAC)
  scheme = AppBundle;
  #else
  if (appDirPath == QLatin1String("/usr/bin")) {
    scheme = UnixStandard;
  }
  else if (appDirPath == QLatin1String("/usr/sbin")) {
    scheme = UnixDaemon;
  }
  else if (QDir(appDirPath).dirName() == QLatin1String("bin")) {
    scheme = UnixAdaptive;
  }
  #endif

  switch (scheme) {
    case Portable:
      m_paths.insert(ConfigPath, appDirPath);
      break;

    case UnixAdaptive:
      m_paths.insert(ConfigPath, QDir::homePath() + QLatin1String("/.config/") + baseName);
      m_paths.insert(SharePath, QDir::cleanPath(appDirPath + QLatin1String("/../share/") + baseName));
      break;

    case UnixStandard:
      m_paths.insert(ConfigPath, QDir::homePath() + QLatin1String("/.config/") + baseName);
      m_paths.insert(SharePath, QLatin1String("/usr/share/") + baseName);
      break;

    case UnixDaemon:
      m_paths.insert(ConfigPath, QLatin1String("/etc/") + baseName);
      m_paths.insert(SharePath, QLatin1String("/usr/share/") + baseName);
      m_paths.insert(VarPath, QLatin1String("/var/lib/") + baseName);
      m_paths.insert(LogPath, QLatin1String("/var/log/") + baseName);
      break;

    case AppBundle:
      m_paths.insert(ConfigPath, QDir::cleanPath(appDirPath + QLatin1String("/../Resources")));
      break;
  }

  if (scheme == Portable || scheme == AppBundle) {
    QString configPath = m_paths.value(ConfigPath);
    m_paths.insert(SharePath, configPath);
    m_paths.insert(VarPath, configPath);
    m_paths.insert(LogPath, configPath);
  }
  else if (scheme == UnixAdaptive || scheme == UnixStandard) {
    m_paths.insert(VarPath, m_paths.value(ConfigPath));
    m_paths.insert(LogPath, m_paths.value(ConfigPath));
    QCoreApplication::addLibraryPath(m_paths.value(ConfigPath) + QLatin1String("/plugins/qt"));
    QCoreApplication::addLibraryPath(m_paths.value(ConfigPath) + QLatin1String("/plugins"));
  }

  m_paths.insert(ConfigFile, m_paths.value(ConfigPath) + QLatin1String("/") + baseName + QLatin1String(".conf"));
  QCoreApplication::addLibraryPath(m_paths.value(SharePath) + QLatin1String("/plugins/qt"));
  QCoreApplication::addLibraryPath(m_paths.value(SharePath) + QLatin1String("/plugins"));

  qDebug() << "---------------------------";
  qDebug() << "AppDirPath" << path(AppDirPath);
  qDebug() << "BaseName  " << path(BaseName);
  qDebug() << "ConfigPath" << path(ConfigPath);
  qDebug() << "ConfigFile" << path(ConfigFile);
  qDebug() << "SharePath " << path(SharePath);
  qDebug() << "VarPath   " << path(VarPath);
  qDebug() << "LogPath   " << path(LogPath);
  qDebug() << "---------------------------";
}


/*!
 * Получение полного имени к файлу.
 * Если \p name относительный путь к файлу, то будет использоваться
 * подсказка \p hint для определения полного имени файла.
 */
QString FileLocations::file(Paths hint, const QString &name) const
{
  if (name.isEmpty())
    return name;

  if (!QFileInfo(name).isRelative())
    return name;

  return QDir::cleanPath(path(hint) + QLatin1String("/") + name);
}
