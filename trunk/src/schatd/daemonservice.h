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

class AbstractProfile;

class DaemonService : public QObject
{
  Q_OBJECT

public:
  DaemonService(QTcpSocket *socket, QObject *parent = 0);
  ~DaemonService();
  bool isReady() const;
  void accessDenied(quint16 reason = 0);
  void accessGranted(quint16 level = 0);
  void privateMessage(quint8 flag, const QString &nick, const QString &message);
  void quit();
  void sendServerMessage(const QString &msg);

signals:
  void greeting(const QStringList &list, quint8 flag);
  void leave(const QString &nick, quint8 flag);
  void message(const QString &channel, const QString &sender, const QString &message);
  void newBye(const QString &nick, const QString &bye);
  void newNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void newProfile(quint8 gender, const QString &nick, const QString &name);

public slots:
  bool newUser(const QStringList &list, bool echo);
  bool userLeave(const QString &nick, const QString &bye, bool echo);
  void disconnected();
  void message(const QString &sender, const QString &message);
  void readyRead();
  void sendLinkLeave(quint8 numeric, const QString &network, const QString &ip);
  void sendNewLink(quint8 numeric, const QString &network, const QString &ip);
  void sendNewNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void sendNewProfile(quint8 gender, const QString &nick, const QString &name);

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
  void opcodePong();
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

#endif /*DAEMONSERVICE_H_*/
