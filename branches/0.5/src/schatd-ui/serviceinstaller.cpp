/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include "serviceinstaller.h"

/*!
 * Конструктор класса ServiceInstaller.
 */
ServiceInstaller::ServiceInstaller(QObject *parent)
  : QObject(parent)
{
}


ServiceInstaller::~ServiceInstaller()
{
  qDebug() << "~ServiceInstaller()";
}


/*!
 * Проверка на наличие сервера с именем \p name в реестре.
 *
 * \return \a true если сервис найден.
 */
bool ServiceInstaller::exists(const QString &name)
{
  QSettings s("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services", QSettings::NativeFormat);

  if (s.value(name + "/Start", -1).toInt() != -1 || !s.value(name + "/ImagePath", "").toString().isEmpty())
    return true;

  return false;
}
