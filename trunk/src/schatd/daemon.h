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

#ifndef DAEMON_H_
#define DAEMON_H_

#include <QTcpServer>
#include <QHash>

class AbstractProfile;
class ChannelLog;
class ClientService;
class DaemonService;
class DaemonSettings;
class LinkUnit;
class Log;
class Network;
class UserUnit;

#define LOG(x, y) if (m_settings->getInt("LogLevel") >= x) m_log->append(y)

class Daemon : public QObject
{
  Q_OBJECT

public:
  Daemon(QObject *parent = 0);
  bool start();
  
signals:
  void message(const QString &sender, const QString &message);
  void newUser(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void sendLinkLeave(quint8 numeric, const QString &network, const QString &ip);
  void sendNewLink(quint8 numeric, const QString &network, const QString &ip);
  void sendNewNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void sendNewProfile(quint8 gender, const QString &nick, const QString &name);
  void sendRelayMessage(const QString &channel, const QString &sender, const QString &message, quint8 numeric = 0);
  void userLeave(const QString &nick, const QString &bye, bool echo);

public slots:
  void incomingConnection();
  
private slots:
  void clientServiceLeave(bool echo = true);
  void clientSyncUsers(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void clientSyncUsersEnd();
  void clientUserLeave(const QString &nick, const QString &bye, bool echo);
  void greeting(const QStringList &list, quint8 flag);
  void linkAccessGranted(const QString &network, const QString &server, quint16 numeric);
  void linkLeave(quint8 numeric, const QString &network, const QString &ip);
  void linkMessage(const QString &sender, const QString &message);
  void linkSyncUsers(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void message(const QString &channel, const QString &sender, const QString &message);
  void newBye(const QString &nick, const QString &bye);
  void newLink(quint8 numeric, const QString &network, const QString &ip);
  void newNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void newProfile(quint8 gender, const QString &nick, const QString &name);
  void relayMessage(const QString &channel, const QString &sender, const QString &msg, quint8 numeric = 0);
  void serviceLeave(const QString &nick, quint8 flag);
  void syncNumerics(const QList<quint8> &numerics);

private:
  bool parseCmd(const QString &nick, const QString &msg);
  QString serverInfo() const;
  void greetingLink(const QStringList &list, DaemonService *service);
  void greetingUser(const QStringList &list, DaemonService *service);
  void link();
  void linkLeave(const QString &nick);
  void sendAllUsers();
  void sendAllUsers(DaemonService *service);
  void userLeave(const QString &nick);

  AbstractProfile *m_profile;
  ChannelLog *m_channelLog;
  ChannelLog *m_privateLog;
  ClientService *m_link;
  DaemonSettings *m_settings;
  Log *m_log;
  Network *m_network;
  QHash<QString, UserUnit *> m_users;
  QHash<quint8, LinkUnit *> m_links;
  QList<quint8> m_numerics;
  QTcpServer m_server;
  quint8 m_numeric;
  quint8 m_remoteNumeric;
};

#endif /*DAEMON_H_*/
