/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
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

/*!
 * \brief Универсальный сервис клиента чата.
 *
 * Класс устанавливает и поддерживает соединение.
 */
class ClientService : public QObject
{
  Q_OBJECT

public:
  ClientService(AbstractProfile *profile, const Network *network, QObject *parent = 0);
  ~ClientService();
  bool isReady() const;
  bool sendMessage(const QString &channel, const QString &message);
  bool sendRelayMessage(const QString &channel, const QString &sender, const QString &message);
  bool sendUniversal(quint16 sub, const QList<quint32> &data1, const QStringList &data2);
  inline QString safeNick() const                                                                            { return m_safeNick; };
  inline void sendByeMsg()                                                                                   { send(OpcodeByeMsg, m_profile->byeMsg()); }
  inline void sendByeMsg(const QString &msg)                                                                 { send(OpcodeByeMsg, msg); }
  inline void sendSyncBye(const QString &nick, const QString &bye)                                           { send(OpcodeSyncByeMsg, nick, bye); }
  inline void sendSyncProfile(quint8 gender, const QString &nick, const QString &nNick, const QString &name) { send(OpcodeNewNick, gender, nick, nNick, name); }
  inline void sendUserLeave(const QString &nick, const QString &bye, quint8 flag)                            { send(OpcodeUserLeave, flag, nick, bye); }
  inline void setSafeNick(const QString &nick)                                                               { m_safeNick = nick; }
  void quit(bool end = true);
  void sendNewUser(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);

signals:
  void accessDenied(quint16 reason);
  void accessGranted(const QString &network, const QString &server, quint16 level);
  void connecting(const QString &server, bool network);
  void fatal();
  void linkLeave(quint8 numeric, const QString &network, const QString &ip);
  void message(const QString &sender, const QString &message);
  void newLink(quint8 numeric, const QString &network, const QString &ip);
  void newNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void newProfile(quint8 gender, const QString &nick, const QString &name);
  void newUser(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void privateMessage(quint8 flag, const QString &nick, const QString &message);
  void relayMessage(const QString &channel, const QString &sender, const QString &message);
  void serverMessage(const QString &msg);
  void syncBye(const QString &nick, const QString &bye);
  void syncNumerics(const QList<quint8> &numerics);
  void syncUsersEnd();
  void unconnected(bool echo = true);
  void universal(quint16 sub, const QList<quint32> &data1, const QStringList &data2);
  void universalLite(quint16 sub, const QList<quint32> &data1);
  void userLeave(const QString &nick, const QString &bye, quint8 flag);

public slots:
  inline void sendNewProfile() { send(OpcodeNewProfile, m_profile->genderNum(), m_profile->nick(), m_profile->fullName()); }
  void connectToHost();

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
  bool send(quint16 opcode, const QString &str1, const QString &str2);
  bool send(quint16 opcode, quint8 gender, const QString &nick, const QString &name);
  bool send(quint16 opcode, quint8 gender, const QString &nick, const QString &nNick, const QString &name);
  int activeInterfaces();
  void createSocket();
  void mangleNick();
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
  void opcodeRelayMessage();
  void opcodeServerMessage();
  void opcodeSyncByeMsg();
  void opcodeSyncNumerics();
  void opcodeSyncUsersEnd();
  void opcodeUniversal();
  void opcodeUserLeave();
  void unknownOpcode();

  AbstractProfile *m_profile;
  bool m_accepted;
  bool m_fatal;
  bool m_synced;
  const Network *m_network;
  int m_reconnects;
  QDataStream m_stream;
  QPointer<QTcpSocket> m_socket;
  QString m_safeNick;
  QTimer m_checkTimer;
  QTimer m_ping;
  QTimer m_reconnectTimer;
  quint16 m_nextBlockSize;
  quint16 m_opcode;
  ServerInfo m_server;
};

#endif /*CLIENTSERVICE_H_*/
