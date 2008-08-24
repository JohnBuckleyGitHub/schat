/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore>

#include "daemonsettings.h"
#include "log.h"

/*!
 * \class DaemonSettings
 * \brief Класс читает настройки сервера.
 * 
 * Читает настройки и предоставляет методы для получения настроек.
 */

/*!
 * \brief Конструктор класса DaemonSettings.
 */
DaemonSettings::DaemonSettings(QObject *parent)
  : QObject(parent)
{
}


/** [public]
 * 
 */
void DaemonSettings::read()
{
  m_settings = new QSettings(QCoreApplication::instance()->applicationDirPath() + "/schatd.conf", QSettings::IniFormat, this);

  readBool("ChannelLog", false);
  readBool("PrivateLog", false);
  readInt("ListenPort", 7666);
  readInt("LogLevel", 0);
  readInt("Numeric", 0);
  readString("ListenAddress", "0.0.0.0");
  readString("NetworkFile", "network.xml");
  readString("Name", "");
}
