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

#ifndef DAEMONSERVICE_H_
#define DAEMONSERVICE_H_

#include <QDataStream>
#include <QTcpSocket>
#include <QTimer>

#include "protocol.h"

class AbstractProfile;

class DaemonService : public QObject
{
  Q_OBJECT

public:
  DaemonService(QTcpSocket *socket, QObject *parent = 0);
  ~DaemonService();
  bool isReady() const;
  inline void sendServerMessage(const QString &msg) { send(OpcodeServerMessage, msg); }
  inline void sendSyncUsersEnd()                    { send(OpcodeSyncUsersEnd); }
  void accessDenied(quint16 reason = 0);
  void accessGranted(quint16 numeric = 0);
  void privateMessage(quint8 flag, const QString &nick, const QString &message);
  void quit();
  void sendNumerics(const QList<quint8> &numerics);

signals:
  void greeting(const QStringList &list, quint8 flag);
  void leave(const QString &nick, quint8 flag);
  void message(const QString &channel, const QString &sender, const QString &message);
  void newBye(const QString &nick, const QString &bye);
  void newNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void newProfile(quint8 gender, const QString &nick, const QString &name);
  void newUser(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void relayMessage(const QString &channel, const QString &sender, const QString &message, quint8 numeric = 0);
  void userLeave(const QString &nick, const QString &bye, quint8 flag);

public slots:
  inline void sendLinkLeave(quint8 numeric, const QString &network, const QString &ip)   { send(OpcodeLinkLeave, numeric, network, ip); }
  inline void sendNewLink(quint8 numeric, const QString &network, const QString &ip)     { send(OpcodeNewLink, numeric, network, ip); }
  inline void sendUserLeave(const QString &nick, const QString &bye, quint8 flag)        { send(OpcodeUserLeave, flag, nick, bye); }
  void disconnected();
  void message(const QString &sender, const QString &message);
  void readyRead();
  void sendNewNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void sendNewProfile(quint8 gender, const QString &nick, const QString &name);
  void sendNewUser(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void sendRelayMessage(const QString &channel, const QString &sender, const QString &message, quint8 numeric = 0);

private slots:
  void ping();

private:
  bool opcodeGreeting();
  bool send(quint16 opcode);
  bool send(quint16 opcode, const QString &msg);
  bool send(quint16 opcode, const QString &str1, const QString &str2);
  bool send(quint16 opcode, quint16 err);
  bool send(quint16 opcode, quint8 flag, const QString &nick, const QString &message);
  bool send(quint16 opcode, quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  quint16 verifyGreeting(quint16 version);
  void opcodeByeMsg();
  void opcodeMessage();
  void opcodeNewProfile();
  void opcodeNewUser();
  void opcodePong();
  void opcodeRelayMessage();
  void opcodeUserLeave();
  void unknownOpcode();

  AbstractProfile *m_profile;
  bool m_accepted;
  int m_pings;
  QDataStream m_stream;
  QTcpSocket *m_socket;
  QTimer m_ping;
  quint16 m_nextBlockSize;
  quint16 m_opcode;
  quint8 m_flag;
  quint8 m_numeric;
};

/*! \fn void DaemonService::sendLinkLeave(quint8 numeric, const QString &network, const QString &ip)
 * \brief Отправка пакета \b OpcodeLinkLeave.
 * 
 * \param numeric Номер сервера подключившегося к сети.
 * \param network Название сети.
 * \param ip Адрес сервера
 */

/*! \fn void DaemonService::sendNewLink(quint8 numeric, const QString &network, const QString &ip)
 * \brief Отправка пакета \b OpcodeNewLink.
 * 
 * \param numeric Номер сервера подключившегося к сети.
 * \param network Название сети.
 * \param ip Адрес сервера
 */

#endif /*DAEMONSERVICE_H_*/
