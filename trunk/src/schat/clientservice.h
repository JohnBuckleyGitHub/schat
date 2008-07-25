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
#include <QTimer>

#include "network.h"

class AbstractProfile;
class Network;

class ClientService : public QObject
{
  Q_OBJECT

public:
  ClientService(const AbstractProfile *profile, const Network *settings, QObject *parent = 0);
  ~ClientService();
  bool isReady();
  bool sendMessage(const QString &channel, const QString &message);
  void connectToHost();
  
signals:
  void accessGranted(const QString &network, const QString &server, quint16 level);
  void connecting(const QString &server, bool network);
  void errorNickAlreadyUse();
  void newUser(const QStringList &list, bool echo);
  void unconnected();
  void userLeave(const QString &nick, const QString &bye, bool echo);
   
private slots:
  void check();
  void connected();
  void disconnected();
  void readyRead();
  void reconnect();

private:
  void createSocket();
  void opcodeAccessDenied();
  void opcodeAccessGranted();
  void opcodeNewUser();
  void opcodeUserLeave();
  
  bool m_accepted;
  bool m_fatal;
  const AbstractProfile *m_profile;
  const Network *m_network;
  int m_reconnects;
  QDataStream m_stream;
  QTcpSocket *m_socket;
  QTimer m_checkTimer;
  QTimer m_reconnectTimer;
  quint16 m_nextBlockSize;
  quint16 m_opcode;
  ServerInfo m_server;
};

#endif /*CLIENTSERVICE_H_*/
