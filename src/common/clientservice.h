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
#include <QPointer>
#include <QTcpSocket>
#include <QTimer>

#include "abstractprofile.h"
#include "network.h"
#include "protocol.h"

class AbstractProfile;
class Network;

class ClientService : public QObject
{
  Q_OBJECT

public:
  ClientService(const AbstractProfile *profile, const Network *settings, QObject *parent = 0);
  ~ClientService();
  bool isReady() const;
  bool sendMessage(const QString &channel, const QString &message);
  inline void sendByeMsg()                   { send(OpcodeByeMsg, m_profile->byeMsg()); }
  inline void sendByeMsg(const QString &msg) { send(OpcodeByeMsg, msg); }
  inline void sendNewProfile()               { send(OpcodeNewProfile, m_profile->genderNum(), m_profile->nick(), m_profile->fullName()); }
  void connectToHost();
  void quit(bool end = true);
  
signals:
  void accessDenied(quint16 reason);
  void accessGranted(const QString &network, const QString &server, quint16 level);
  void connecting(const QString &server, bool network);
  void fatal();
  void linkLeave(quint8 numeric, const QString &network, const QString &ip);
  void message(const QString &sender, const QString &message);
  void newLink(quint8 numeric, const QString &network, const QString &ip);
  void newNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void newProfile(quint8 gender, const QString &nick, const QString &name, bool echo);
  void newUser(const QStringList &list, bool echo);
  void privateMessage(quint8 flag, const QString &nick, const QString &message);
  void serverMessage(const QString &msg);
  void unconnected(bool echo = true);
  void userLeave(const QString &nick, const QString &bye, bool echo);

private slots:
  void check();
  void connected();
  void disconnected();
  void ping();
  void readyRead();
  void reconnect();

private:
  bool send(quint16 opcode);
  bool send(quint16 opcode, const QString &msg);
  bool send(quint16 opcode, quint8 gender, const QString &nick, const QString &name);
  void createSocket();
  void opcodeAccessDenied();
  void opcodeAccessGranted();
  void opcodeLinkLeave();
  void opcodeMessage();
  void opcodeNewLink();
  void opcodeNewNick();
  void opcodeNewProfile();
  void opcodeNewUser();
  void opcodePing();
  void opcodePrivateMessage();
  void opcodeServerMessage();
  void opcodeUserLeave();
  void unknownOpcode();
  
  bool m_accepted;
  bool m_fatal;
  const AbstractProfile *m_profile;
  const Network *m_network;
  int m_reconnects;
  QDataStream m_stream;
  QPointer<QTcpSocket> m_socket;
  QTimer m_checkTimer;
  QTimer m_ping;
  QTimer m_reconnectTimer;
  quint16 m_nextBlockSize;
  quint16 m_opcode;
  ServerInfo m_server;
};

#endif /*CLIENTSERVICE_H_*/
