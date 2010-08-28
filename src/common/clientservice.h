/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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
#include <QTimer>

#include "AbstractPeer.h"
#include "abstractprofile.h"
#include "network.h"
#include "protocol.h"

class PacketBuilder;
class PacketReader;

/*!
 * \brief Универсальный сервис клиента чата.
 *
 * Класс устанавливает и поддерживает соединение.
 */
class ClientService : public AbstractPeer
{
  Q_OBJECT

public:
  ClientService(AbstractProfile *profile, const Network *network, QObject *parent = 0);
  ~ClientService();
  bool sendMessage(const QString &channel, const QString &message);
  bool sendRelayMessage(const QString &channel, const QString &sender, const QString &message);
  bool sendUniversal(quint16 sub, const QList<quint32> &data1, const QStringList &data2);
  inline QString safeNick() const                                                                      { return m_safeNick; };
  inline void sendByeMsg()                                                                             { sendByeMsg(m_profile->byeMsg()); }
  inline void setSafeNick(const QString &nick)                                                         { m_safeNick = nick; }
  void quit(bool end = true);
  void sendByeMsg(const QString &msg);
  void sendNewUser(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void sendSyncBye(const QString &nick, const QString &bye);
  void sendSyncProfile(quint8 gender, const QString &nick, const QString &nNick, const QString &name);
  void sendUserLeave(const QString &nick, const QString &bye, quint8 flag);

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
  void connectToHost();
  void sendNewProfile();

protected:
  void readPacket(int pcode, const QByteArray &block);

private slots:
  void check();
  void connected();
  void disconnected();
  void ping();
  void reconnect();

private:
  int activeInterfaces();
  void createSocket();
  void mangleNick();
  void messagePacket(const PacketReader &reader);
  void opcodeAccessDenied(const PacketReader &reader);
  void opcodeAccessGranted(const PacketReader &reader);
  void opcodeLinkLeave(const PacketReader &reader);
  void opcodeNewLink(const PacketReader &reader);
  void opcodeNewNick(const PacketReader &reader);
  void opcodeNewProfile(const PacketReader &reader);
  void opcodeNewUser(const PacketReader &reader);
  void opcodePing();
  void opcodePrivateMessage(const PacketReader &reader);
  void opcodeRelayMessage(const PacketReader &reader);
  void opcodeServerMessage(const PacketReader &reader);
  void opcodeSyncByeMsg(const PacketReader &reader);
  void opcodeSyncNumerics(const PacketReader &reader);
  void opcodeUniversal(const PacketReader &reader);
  void opcodeUserLeave(const PacketReader &reader);

  AbstractProfile *m_profile;
  bool m_accepted;
  bool m_fatal;
  const Network *m_network;
  int m_reconnects;
  QString m_safeNick;
  QTimer m_checkTimer;
  QTimer m_ping;
  QTimer m_reconnectTimer;
  quint16 m_opcode;
  ServerInfo m_server;
};

#endif /*CLIENTSERVICE_H_*/
