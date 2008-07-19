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

#include "abstractprofile.h"
#include "daemonservice.h"
#include "userunit.h"


/** [public]
 * 
 */
UserUnit::UserUnit()
{
  m_profile = 0;
  m_service = 0;
}


/** [public]
 * 
 */
UserUnit::UserUnit(const QStringList &list, DaemonService *service)
{
  m_profile = new AbstractProfile(list);
  m_service = service;
}


/** [public]
 * 
 */
UserUnit::~UserUnit()
{
  qDebug() << "UserUnit::~UserUnit()";
  
  if (m_profile)
    delete m_profile;
}
