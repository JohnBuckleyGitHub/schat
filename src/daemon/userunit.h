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

#ifndef USERUNIT_H_
#define USERUNIT_H_

#include <QPointer>

#include "daemonservice.h"

class AbstractProfile;

class UserUnit {
  
public:
  UserUnit();
  UserUnit(const QStringList &list, DaemonService *service = 0);
  ~UserUnit();
  inline AbstractProfile* profile() { return m_profile; }
  inline DaemonService* service()   { return m_service; }
  
private:
  AbstractProfile *m_profile;
  QPointer<DaemonService> m_service;
};

#endif /*USERUNIT_H_*/
