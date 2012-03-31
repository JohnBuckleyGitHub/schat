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

#include "sglobal.h"
#include "tools/OsInfo.h"

int OsInfo::m_type = -1;
QVariantMap OsInfo::m_json;

int OsInfo::type()
{
  if (m_type == -1)
    init();

  return m_type;
}


QVariantMap OsInfo::json()
{
  if (m_type == -1)
    init();

  return m_json;
}


void OsInfo::init()
{
  QString os;
# if defined(Q_OS_WIN32)
  m_type = Windows;
  switch (QSysInfo::WindowsVersion) {
    case QSysInfo::WV_WINDOWS7:
      os = LS("Windows 7");
      break;

    case QSysInfo::WV_VISTA:
      os = LS("Windows Vista");
      break;

    case QSysInfo::WV_2003:
      os = LS("Windows Server 2003");
      break;

    case QSysInfo::WV_XP:
      os = LS("Windows XP");
      break;

    case QSysInfo::WV_2000:
      os = LS("Windows 2000");
      break;

    default:
      os = LS("Windows");
      break;
  }
# elif defined(Q_OS_MAC)
  m_type = MacOSX;
  switch (QSysInfo::MacintoshVersion) {
    case QSysInfo::MV_LION:
      os = LS("Mac OS X 10.7 Lion");

    case QSysInfo::MV_SNOWLEOPARD:
      os = LS("Mac OS X 10.6 Snow Leopard");

    case QSysInfo::MV_LEOPARD:
      os = LS("Mac OS X 10.5 Leopard");

    default:
      os = LS("Mac OS X");
      break;
  }
# elif defined(Q_OS_LINUX)
  m_type = Linux;
# else
  m_type = Unknown;
# endif

  m_json[LS("os")] = os;
}
