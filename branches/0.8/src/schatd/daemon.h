/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#ifndef DAEMON_H_
#define DAEMON_H_

#include <QHash>
#include <QPointer>
#include <QTcpServer>
#include <QTimer>

#include "network.h"
#include "userunit.h"

class AbstractProfile;
class ChannelLog;
class ClientService;
class DaemonLog;
class DaemonService;
class DaemonSettings;
class FloodOfflineItem;
class LinkUnit;
class Network;
class QLocalServer;
class UserUnit;
class Translation;

#define SCHATD_LOG(x, y, z) if (logLevel >= x) m_log->append(y, z)

/*!
 * \brief Сервер чата
 *
 * Класс полностью включает в себя функциональность сервера чата.
 */
class Daemon : public QObject
{
  Q_OBJECT

public:
  Daemon(QObject *parent = 0);
  ~Daemon();
  bool start();
  static Daemon *instance() { return m_self; }
  void reload(int code);

signals:
  void newUser(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void sendLinkLeave(quint8 numeric, const QString &network, const QString &name);
  void sendMessage(const QString &sender, const QString &message);
  void sendNewLink(quint8 numeric, const QString &network, const QString &ip);
  void sendNewNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void sendNewProfile(quint8 gender, const QString &nick, const QString &name);
  void sendRelayMessage(const QString &channel, const QString &sender, const QString &message, quint8 numeric = 0);
  void sendSyncBye(const QString &nick, const QString &bye);
  void sendSyncProfile(quint8 gender, const QString &nick, const QString &newNick, const QString &name);
  void sendUniversal(quint16 sub, const QList<quint32> &data1, const QStringList &data2);
  void userLeave(const QString &nick, const QString &bye, quint8 flag);

private slots:
  inline void newBye(const QString &nick, const QString &bye)                                            { syncBye(nick, bye, true); }
  inline void newNick(quint8 gender, const QString &nick, const QString &nNick, const QString &name)     { syncProfile(gender, nick, nNick, name, true); }
  inline void newProfile(quint8 gender, const QString &nick, const QString &name)                        { syncProfile(gender, nick, "", name, true); }
  inline void syncBye(const QString &nick, const QString &bye)                                           { syncBye(nick, bye, false); }
  inline void syncProfile(quint8 gender, const QString &nick, const QString &nNick, const QString &name) { syncProfile(gender, nick, nNick, name, false); }
  void clientAccessGranted(const QString &network, const QString &server, quint16 numeric);
  void clientServiceLeave(bool echo = true);
  void clientSyncUsers(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void clientSyncUsersEnd();
  void clientUserLeave(const QString &nick, const QString &bye, quint8 flag);
  void detectZombie();
  void dumpStats();
  void greeting(const QStringList &list, quint8 flag);
  void incomingConnection();
  void linkLeave(quint8 numeric, const QString &network, const QString &ip);
  void logLinkLeave(quint8 numeric, const QString &network, const QString &name);
  void logMessage(const QString &sender, const QString &message);
  void logNewLink(quint8 numeric, const QString &network, const QString &name);
  void logNewUser(const QStringList &list, quint8 echo = 1, quint8 numeric = 0);
  void message(const QString &channel, const QString &sender, const QString &message);
  void newLink(quint8 numeric, const QString &network, const QString &ip);
  void relayMessage(const QString &channel, const QString &sender, const QString &msg);
  void serviceLeave(const QString &nick, quint8 flag, const QString &err);
  void syncNumerics(const QList<quint8> &numerics);
  void universal(quint16 sub, const QList<quint32> &data1, const QStringList &data2, quint8 numeric = 0);
  void universal(quint16 sub, const QString &nick, const QList<quint32> &data1, const QStringList &data2);

  #ifndef SCHAT_NO_LOCAL_SERVER
    void incomingLocalConnection();
  #endif

private:
  /// Конфигурационные опции задаваемые через переменные окружения.
  enum Env {
    EnvConfFile, ///< Полное имя конфигурационного файла.
    EnvPidFile,  ///< Полное имя PID-файла.
    EnvLogDir,   ///< Директория для записи журналов сервера.
    EnvVarDir,   ///< Директория для файлов изменяемых в процессе работы.
    EnvShare     ///< Директория для общих файлов.
  };

  bool initMigrate();
  bool initMotd();
  bool initStats();
  bool parseCmd(const QString &nick, const QString &msg);
  int localLinksCount() const;
  int localUsersCount() const;
  QString envConfFile(const QString &key) const;
  QString normalizeNick(const QString &nick) const;
  QString serverInfo() const;
  quint16 greetingLink(const QStringList &list, DaemonService *service);
  quint16 greetingUser(const QStringList &list, DaemonService *service);
  static QString envValue(const QString &env, const QString &failBack);
  void environment();
  void link();
  void linkLeave(const QString &nick, const QString &err);
  void removeUser(const QString &nick, const QString &err = QString(), quint8 flag = 1);
  void sendAllUsers(DaemonService *service);
  void syncBye(const QString &nick, const QString &bye, bool local);
  void syncProfile(quint8 gender, const QString &nick, const QString &nNick, const QString &name, bool local);
  void updateStatus(quint32 status, const QStringList &users);

  AbstractProfile *m_profile;
  bool m_motd;
  bool m_stats;
  bool m_syncUsers;
  ChannelLog *m_channelLog;
  ChannelLog *m_privateLog;
  ClientService *m_link;
  DaemonLog *m_log;
  DaemonSettings *m_settings;
  FloodLimits m_floodLimits;
  int logLevel;
  int m_maxLinks;
  int m_maxUsers;
  int m_maxUsersPerIp;
  int m_statsInterval;
  QHash<QChar, QChar> m_normalize;
  QHash<QString, FloodOfflineItem> m_floodOffline;
  QHash<QString, int> m_ipLimits;
  QHash<QString, UserUnit *> m_users;
  QHash<quint8, LinkUnit *> m_links;
  QList<quint8> m_numerics;
  QMap<Env, QString> m_environment;
  QPointer<Network> m_network;
  QString m_migrate; ///< JSON данные для автоматической миграции на Simple Chat 2.
  QString m_motdText;
  QString m_statsFile;
  QTcpServer m_server;
  QTimer zombieTimer;
  quint8 m_numeric;
  quint8 m_remoteNumeric;
  static Daemon *m_self;
  Translation *m_translation;

  #ifndef SCHAT_NO_LOCAL_SERVER
    QLocalServer *m_localServer;
  #endif
};

/*! \fn void Daemon::sendLinkLeave(quint8 numeric, const QString &network, const QString &ip)
 * \brief Уведомление об отключении от данного сервера другого сервера.
 *
 * \param numeric Номер сервера подключившегося к сети.
 * \param network Название сети.
 * \param ip Адрес сервера
 */

/*! \fn void Daemon::sendNewLink(quint8 numeric, const QString &network, const QString &ip)
 * \brief Уведомление о подключении к данному серверу другого сервера.
 *
 * \param numeric Номер сервера подключившегося к сети.
 * \param network Название сети.
 * \param ip Адрес сервера
 */

#endif /*DAEMON_H_*/
