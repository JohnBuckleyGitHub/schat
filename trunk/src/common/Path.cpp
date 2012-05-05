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

#include <QDebug>

#include <QCoreApplication>
#include <QFileInfo>
#include <QSettings>

#include "Path.h"
#include "sglobal.h"

bool Path::m_portable = false;
QString Path::m_app;
QString Path::m_appDirPath;

void Path::init()
{
  m_appDirPath = QCoreApplication::applicationDirPath();
  m_app = QFileInfo(QCoreApplication::applicationFilePath()).baseName();

  QSettings s(m_appDirPath + LC('/') + m_app + LS(".init"), QSettings::IniFormat);
  s.setIniCodec("UTF-8");
  m_portable = s.value(LS("Portable"), false).toBool();
}


/*!
 * Возвращает имя конфигурационного файла.
 */
QString Path::config()
{
  return data() + LC('/') + app() + LS(".conf");
}
