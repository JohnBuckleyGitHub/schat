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

#include "daemon.h"
#include "daemonservice.h"


/** [public]
 * 
 */
Daemon::Daemon(QObject *parent)
  : QObject(parent)
{
  connect(&m_server, SIGNAL(newConnection()), SLOT(incomingConnection()));
}


/** [public]
 * 
 */
bool Daemon::start()
{
  QString address = "0.0.0.0"; // FIXME эти данные должны браться из настроек
  quint16 port    = 7667;  
  bool result     = m_server.listen(QHostAddress(address), port);
  
  return result;
}


/** [public slots]
 * 
 */
void Daemon::incomingConnection()
{
  if (m_server.hasPendingConnections()) {
    DaemonService *service = new DaemonService(m_server.nextPendingConnection(), this);
    connect(service, SIGNAL(greeting(const QStringList &)), SLOT(greeting(const QStringList &)));
  }
}


/** [private slots]
 * 
 */
void Daemon::greeting(const QStringList &list)
{
  qDebug() << "Daemon::greeting(const QStringList &)" << list;
  
}
