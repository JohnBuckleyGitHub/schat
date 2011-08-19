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

#ifndef CORE_H_
#define CORE_H_

#include <QHash>
#include <QObject>
#include <QVariant>

#include "ServerChannel.h"
#include "ServerUser.h"

class AuthResult;
class MessageData;
class NewPacketsEvent;
class NodeAuth;
class PacketReader;
class Settings;
class SocketReleaseEvent;
class Storage;
class Worker;
class WorkerThread;

class SCHAT_EXPORT Core : public QObject
{
  Q_OBJECT

public:
  Core(QObject *parent = 0);
  ~Core();
  inline NewPacketsEvent *packetsEvent() { return m_packetsEvent; }
  inline QByteArray readBuffer() const { return m_readBuffer; }
  inline void addAuth(NodeAuth *auth) { m_auth.prepend(auth); }
  virtual int start() { return 0; }
  virtual void quit() {}

public slots:
  inline void workersReady(QObject *listener) { m_listener = listener; }

protected:
  void customEvent(QEvent *event);

protected:
  bool broadcast();
  bool route();
  bool route(ChatChannel channel);
  bool route(ChatUser user);
  bool send(ChatChannel channel, const QByteArray &packet);
  bool send(ChatChannel channel, const QList<QByteArray> &packets);
  bool send(ChatUser user, const QByteArray &packet, int option = 0);
  bool send(ChatUser user, const QList<QByteArray> &packets, int option = 0);
  bool send(const QList<quint64> &sockets, const QByteArray &packet);
  bool send(const QList<quint64> &sockets, const QList<QByteArray> &packets);
  qint64 timestamp() const;
  void slaveBroadcast();

  virtual bool checkPacket();
  virtual void newPacketsEvent(NewPacketsEvent *event);
  virtual void readPacket(int type);
  virtual void socketReleaseEvent(SocketReleaseEvent *event);

  bool join(const QByteArray &userId, ChatChannel channel);
  bool join(const QByteArray &userId, const QByteArray &channelId);
  ChatChannel addChannel(ChatUser user);
  ChatChannel channel(const QString &name, bool create = true);
  QList<quint64> echoFilter(const QList<quint64> &sockets);

  // Авторизация.
  bool readUserData();
  virtual bool readAuthRequest();
  virtual void acceptAuth(const AuthResult &result);
  virtual void rejectAuth(const AuthResult &result);
  void sendChannel(ChatChannel channel, ChatUser user);

  // u2u.
  void addTalk(ChatUser user1, ChatUser user2);
  void bindTalks();
  void bindTalks(ChatUser senderUser, ChatUser destUser);

  // users.
  bool updateUserStatus();

  // messages.
  bool command();
  bool readJoinCmd();
  bool readMessage();
  void acceptMessage();
  void rejectMessage(int reason);

  MessageData *m_messageData;         ///< Текущий прочитанный объект MessageData.
  NewPacketsEvent *m_packetsEvent;    ///< Текущий объект NewPacketsEvent.
  PacketReader *m_reader;             ///< Текущий объект PacketReader выполняющий чтение пакета.
  QByteArray m_readBuffer;            ///< Буфер чтения виртуальных пакетов.
  QByteArray m_sendBuffer;            ///< Буфер отправки виртуальных пакетов.
  QDataStream *m_readStream;          ///< Поток чтения виртуальных пакетов.
  QDataStream *m_sendStream;          ///< Поток отправки виртуальных пакетов.
  qint64 m_timestamp;                 ///< Отметка времени.
  QList<NodeAuth *> m_auth;           ///< Модули авторизации.
  QObject *m_listener;                ///< Слушатель сообщений.
  Settings *m_settings;               ///< Настройки.
  Storage *m_storage;                 ///< Хранилище данных.
};

#endif /* CORE_H_ */
