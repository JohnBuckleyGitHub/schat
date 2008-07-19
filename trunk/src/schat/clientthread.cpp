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

#include "clientthread.h"
#include "clientservice.h"


/** [public]
 * 
 */
ClientThread::ClientThread(const QStringList &profile, const QString &server, quint16 port, QObject *parent)
  : QThread(parent)
{
  m_service = new ClientService(profile, server, port);
}


/** [public]
 * 
 */
ClientThread::~ClientThread()
{
  if (m_service)
    delete m_service;
  
   exit();
   wait();
}


/** [public]
 * 
 */
void ClientThread::run()
{
  qDebug() << "ClientThread::run()";
  m_service->connectToHost();
  
  exec();
}
