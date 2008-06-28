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

#ifndef SERVER_H_
#define SERVER_H_

#include <QHash>
#include <QTcpServer>

#include "profile.h"
#include "serversocket.h"

class DaemonSettings;

class Server : public QTcpServer
{
  Q_OBJECT

public:
  Server(QObject *parent = 0);
  bool start();
  
  #ifdef SCHAT_CLIENT
  void setLocalProfile(Profile *p) { localProfile = p; }
  #endif
  
signals:
  void incomingDirectConnection(const QString &p, ServerSocket *socket);

public slots:
  void appendParticipant(const QString &p);
  void clientSendNewProfile(quint16 sex, const QString &nick, const QString &name);
  void relayMessage(const QString &channel, const QString &nick, const QString &message);
  void relayParticipantList(ServerSocket *socket);
  
  #ifdef SCHAT_CLIENT
  void appendDirectParticipant(const QString &p);
  #endif
  
private slots:
  void connectionError(QAbstractSocket::SocketError socketError);
  void disconnected();

private:
  void incomingConnection(int socketId);
  void participantLeft(const QString &nick);
  void removeConnection(ServerSocket *socket);
  
  DaemonSettings *m_settings;
  QHash<QString, ServerSocket *> peers;
  
  #ifdef SCHAT_CLIENT
  void removeDirectConnection(ServerSocket *socket);
  Profile *localProfile;
  QHash<QString, ServerSocket *> directPeers;
  #endif
};

#endif /*SERVER_H_*/
