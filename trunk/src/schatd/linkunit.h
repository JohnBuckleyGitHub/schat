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

#ifndef LINKUNIT_H_
#define LINKUNIT_H_

#include <QPointer>

#include "daemonservice.h"

class AbstractProfile;

class LinkUnit {
  
public:
  LinkUnit(quint8 numeric, DaemonService *service = 0);
  ~LinkUnit();
  inline DaemonService* service() { return m_service; }
  inline quint8 numeric() const   { return m_numeric; } 

private:
  QPointer<DaemonService> m_service;
  quint8 m_numeric;
};

#endif /*LINKUNIT_H_*/
