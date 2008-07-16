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

#include <QtNetwork>

#include "daemonthread.h"


static const int Timeout = 5 * 1000;


/** [public]
 * Конструктор
 */
DaemonThread::DaemonThread(int socketDescriptor, QObject *parent)
  : QThread(parent)
{
  m_descriptor = socketDescriptor;
  m_quit = false;
  m_socket = 0;
}


/** [public]
 * Деструктор
 */
DaemonThread::~DaemonThread()
{
  qDebug() << "DaemonThread::~DaemonThread()";
  if (m_socket)
    delete m_socket;  
}


/** [public]
 * 
 */
void DaemonThread::run()
{
  qDebug() << "DaemonThread::run()";
  
  m_socket = new QTcpSocket;
  connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
  
  if (!m_socket->setSocketDescriptor(m_descriptor)) {
    qDebug() << "ERR: NOT SET SOCKET DESCRIPTOR";
    emit error(m_socket->error());
    return;
  }
  
  if (!m_socket->waitForReadyRead(Timeout)) {
    qDebug() << "ERR: WAIT FOR READY READ TIMEOUT";
    emit error(m_socket->error());
    return;
  }

  exec();
}


/** [private slots]
 * 
 */
void DaemonThread::readyRead()
{
  qDebug() << "DaemonThread::readyRead()";
  exit();
}
