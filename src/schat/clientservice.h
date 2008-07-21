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

#ifndef CLIENTSERVICE_H_
#define CLIENTSERVICE_H_

#include <QObject>
#include <QTcpSocket>

class AbstractProfile;
class Network;

class ClientService : public QObject
{
  Q_OBJECT

public:
  ClientService(const AbstractProfile *profile, const Network *settings, QObject *parent = 0);
  ~ClientService();
  void connectToHost();
  
signals:
  void connecting(const QString &server, bool network);
   
private slots:
  void connected();
  void disconnected();
  void readyRead();

private:
  void createSocket();
  
  const AbstractProfile *m_profile;
  const Network *m_network;
  QTcpSocket *m_socket;
};

#endif /*CLIENTSERVICE_H_*/
