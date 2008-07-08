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


/** [public]
 * 
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
  QSettings s(QCoreApplication::instance()->applicationDirPath() + "/schatd.conf", QSettings::IniFormat, this);
  
  listenAddress = s.value("ListenAddress", "0.0.0.0").toString();
  listenPort    = quint16(s.value("ListenPort", 7666).toUInt());
  
  #ifdef SCHAT_CLIENT
  logLevel = s.value("LogLevel", -1).toInt();
  #else
  logLevel = s.value("LogLevel", 0).toInt();
  #endif
  if (logLevel > -1) {
    log = new Log(this);
    if (!log->init()) {
      log->deleteLater();
      logLevel = -1;
    }
  }
}
