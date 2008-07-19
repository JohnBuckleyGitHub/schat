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
#include <QtNetwork>

#include "abstractprofile.h"
#include "clientservice.h"


/** [public]
 * 
 */
ClientService::ClientService(const QStringList &profile, const QString &server, quint16 port, QObject *parent)
  :QObject(parent), m_server(server), m_port(port)
{
  m_profile = new AbstractProfile(profile, this);
}


/** [public]
 * 
 */
ClientService::~ClientService()
{
  qDebug() << "ClientService::~ClientService()";
}


/** [public]
 * 
 */
bool ClientService::connectToHost()
{
  qDebug() << "ClientService::access()";
  
  m_socket.connectToHost(m_server, m_port);
  if (!m_socket.waitForConnected(5000))
    return false;
  
  return true;
}
