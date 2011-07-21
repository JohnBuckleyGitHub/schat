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

#ifndef SIMPLECLIENT_H_
#define SIMPLECLIENT_H_

#include <QUrl>

#include "net/SimpleSocket.h"
#include "User.h"
#include "schat.h"

class Channel;
class ClientOfflineCache;
class MessageData;
class NoticeData;
class Packet;
class PacketReader;
class ServerData;
class SyncChannelCache;
class User;

class SCHAT_EXPORT SimpleClient : public SimpleSocket
{
  Q_OBJECT

public:
  /// Состояние клиента.
  enum ClientState {
    ClientOnline,      ///< Клиент успешно подключен
    ClientOffline,     ///< Клиент отключен.
    ClientConnecting,  ///< Клиент в состоянии подключения к серверу.
    ClientError        ///< Критическая ошибка.
  };

  SimpleClient(User *user, quint64 id, QObject *parent = 0);
  ~SimpleClient();

  bool openUrl(const QUrl &url);
  bool send(const MessageData &data);
  bool send(const QByteArray &packet);
  bool send(const QList<QByteArray> &packets);
  inline bool openUrl(const QString &url) { return openUrl(QUrl(url)); }
  inline Channel* channel(const QByteArray &id) const { return m_channels.value(id); }
  inline ClientUser user() const { return m_user; }
  inline ClientUser user(const QByteArray &id) const { return m_users.value(id); }
  inline ClientState clientState() const { return m_clientState; }
  inline QUrl url() const { return m_url; }
  inline ServerData *serverData() { return m_serverData; }
  QByteArray serverId() const;
  QString nick() const;
  void setNick(const QString &nick);

  void leave();
  void part(const QByteArray &channelId);

signals:
  void clientStateChanged(int state);
  void join(const QByteArray &channelId, const QByteArray &userId);
  void join(const QByteArray &channelId, const QList<QByteArray> &usersId);
  void message(const MessageData &data);
  void notice(const NoticeData &data);
  void part(const QByteArray &channelId, const QByteArray &userId);
  void synced(const QByteArray &channelId);
  void userDataChanged(const QByteArray &userId);
  void userLeave(const QByteArray &userId);

protected:
  void newPacketsImpl();
  void timerEvent(QTimerEvent *event);

private slots:
  void released();
  void requestAuth();

private:
  bool addChannel(Channel *channel);
  inline void lock() { m_sendLock = true; }
  QString mangleNick();
  void clearClient();
  void restore();
  void setClientState(ClientState state);
  void setServerData(const ServerData &data);
  void unlock();

  bool command();
  bool readAuthReply();
  bool readChannel();
  bool readMessage();
  bool readNotice();

  // m_users.
  bool readUserData();
  bool removeUser(const QByteArray &userId);
  bool removeUserFromChannel(const QByteArray &channelId, const QByteArray &userId, bool clear = true);
  void updateUserData(ClientUser existUser, User *user);
  void updateUserStatus(const QString &text);

  bool m_sendLock;                           ///< Блокировка отправки пакетов, пакеты будут добавлены в очередь и будут отправлены после снятия блокировки.
  ClientUser m_user;                           ///< Пользователь.
  ClientState m_clientState;                 ///< Состояние клиента.
  int m_reconnects;                          ///< Число попыток восстановить соединение.
  MessageData *m_messageData;                ///< Текущий прочитанный объект MessageData.
  PacketReader *m_reader;                    ///< Текущий объект PacketReader выполняющий чтение пакета.
  QBasicTimer *m_reconnectTimer;             ///< Таймер управляющий попытками переподключения.
  QByteArray m_uniqueId;                     ///< Уникальный идентификатор пользователя.
  QHash<QByteArray, Channel*> m_channels;    ///< Таблица каналов.
  QHash<QByteArray, ClientUser> m_users;       ///< Таблица пользователей.
  QList<QByteArray> m_sendQueue;             ///< Список виртуальных пакетов, ожидающих отправки если установлена блокировка на отправку.
  QString m_nick;                            ///< Оригинальный ник пользователя.
  QUrl m_url;                                ///< Адрес, к которому будет подключен клиент.
  ServerData *m_serverData;                  ///< Данные о сервере.
  SyncChannelCache *m_syncChannelCache;      ///< Данные синхронизации канала.
};

#endif /* SIMPLECLIENT_H_ */
