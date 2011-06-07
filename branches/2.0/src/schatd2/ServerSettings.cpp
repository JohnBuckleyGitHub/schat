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

#include <QDir>

#include "ServerSettings.h"

ServerSettings::ServerSettings(QObject *parent)
  : Settings(parent)
{
  if (scheme() == UnixAdaptive) {
    m_root = QDir::cleanPath(m_appDirPath + "/../etc/" + m_baseName);
    m_var = QDir::cleanPath(m_appDirPath + "/../var/" + m_baseName);
  }
  else if (scheme() == UnixStandard) {
    m_root = "/etc/" + m_baseName;
    m_var = "/var/lib/" + m_baseName;
  }
  else
    m_var = m_root;
}
