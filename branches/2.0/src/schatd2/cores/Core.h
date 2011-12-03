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
class Notice;
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

  // Функции отправки пакетов.
  virtual bool route();
  virtual bool send(ChatUser user, const QByteArray &packet, int option = 0);
  virtual bool send(ChatUser user, const QList<QByteArray> &packets, int option = 0);
  virtual bool send(const QList<quint64> &sockets, const QByteArray &packet, int option = 0, const QByteArray &userId = QByteArray());
  virtual bool send(const QList<quint64> &sockets, const QList<QByteArray> &packets, int option = 0, const QByteArray &userId = QByteArray());

  inline NewPacketsEvent *packetsEvent() { return m_packetsEvent; }
  inline QByteArray readBuffer() const { return m_readBuffer; }
  inline QDataStream *sendStream() { return m_sendStream; }
  inline void addAuth(NodeAuth *auth) { m_auth.prepend(auth); }
  inline void setPlugins(NodePlugins *plugins) { m_plugins = plugins; }
  QByteArray id() const;
  virtual bool add(ChatChannel channel, int authType, const QByteArray &authId);
  virtual int start() { return 0; }
  virtual void quit() {}

public slots:
  inline void workersReady(QObject *listener) { m_listener = listener; }

protected:
  void customEvent(QEvent *event);

protected:
  virtual bool checkPacket();
  virtual void newPacketsEvent(NewPacketsEvent *event);
  virtual void packet(int type);

  QList<QByteArray> userDataToSync(ChatChannel channel, ChatUser user);

  // Авторизация.
  virtual bool auth();
  virtual void accept(const AuthResult &result);
  virtual void reject(const AuthResult &result);

  // users.
  bool readUserData();
  bool updateUserData(ChatUser user, User *other);
  bool updateUserStatus();
  void release(SocketReleaseEvent *event);

  // messages.
  bool command();
  bool message();
  bool readTopic();
  void acceptMessage(int status = 200);
  void rejectMessage(int status);

  // notices.
  bool login();
  bool reg();
  virtual void notice(quint16 type);
  void rejectNotice(int status);

  virtual void acceptedMessageHook(int reason);
  virtual void userReadyHook();

  MessageData *m_messageData;         ///< Текущий прочитанный объект MessageData.
  NewPacketsEvent *m_packetsEvent;    ///< Текущий объект NewPacketsEvent.
  NodePlugins *m_plugins;             ///< Плагины.
  Notice *m_notice;                   ///< Текущий прочитанный объект Notice.
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
