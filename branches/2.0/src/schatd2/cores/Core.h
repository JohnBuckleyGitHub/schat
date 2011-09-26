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
class NodePlugins;
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
  bool send(ChatChannel channel, const QByteArray &packet);
  bool send(ChatChannel channel, const QList<QByteArray> &packets);
  bool send(ChatUser user, const QByteArray &packet, int option = 0);
  bool send(ChatUser user, const QList<QByteArray> &packets, int option = 0);
  bool send(const QList<quint64> &sockets, const QByteArray &packet);
  bool send(const QList<quint64> &sockets, const QList<QByteArray> &packets);
  inline NewPacketsEvent *packetsEvent() { return m_packetsEvent; }
  inline QByteArray readBuffer() const { return m_readBuffer; }
  inline QDataStream *sendStream() { return m_sendStream; }
  inline void addAuth(NodeAuth *auth) { m_auth.prepend(auth); }
  inline void setPlugins(NodePlugins *plugins) { m_plugins = plugins; }
  virtual bool add(ChatUser user, int authType);
  virtual int start() { return 0; }
  virtual void quit() {}

public slots:
  inline void workersReady(QObject *listener) { m_listener = listener; }

protected:
  void customEvent(QEvent *event);

protected:
  bool route();

  virtual bool checkPacket();
  virtual void newPacketsEvent(NewPacketsEvent *event);
  virtual void readPacket(int type);
  virtual void socketReleaseEvent(SocketReleaseEvent *event);

  bool join(const QByteArray &userId, ChatChannel channel);
  ChatChannel addChannel(ChatUser user);
  QList<QByteArray> userDataToSync(ChatChannel channel, ChatUser user);
  QList<quint64> echoFilter(const QList<quint64> &sockets);

  // Авторизация.
  virtual bool readAuthRequest();
  virtual void acceptAuth(const AuthResult &result);
  virtual void rejectAuth(const AuthResult &result);

  // users.
  bool readUserData();
  bool updateUserData(ChatUser user, User *other);
  bool updateUserStatus();

  // messages.
  bool command();
  bool readJoinCmd();
  bool readLeaveCmd();
  bool readMessage();
  bool readTopic();
  void acceptMessage(int reason = 0);
  void rejectMessage(int reason);

  virtual void acceptedMessageHook(int reason);
  virtual void userReadyHook();

  MessageData *m_messageData;         ///< Текущий прочитанный объект MessageData.
  NewPacketsEvent *m_packetsEvent;    ///< Текущий объект NewPacketsEvent.
  NodePlugins *m_plugins;             ///< Плагины.
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
