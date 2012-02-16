/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include "net/packets/Notice.h"
#include "ServerChannel.h"

class AuthResult;
class NewPacketsEvent;
class NodeAuth;
class Notice;
class PacketReader;
class Settings;
class SocketReleaseEvent;
class Storage;
class Worker;

class SCHAT_EXPORT Core : public QObject
{
  Q_OBJECT

public:
  Core(QObject *parent = 0);
  ~Core();
  inline static Core *i() { return m_self; }

  // Функции отправки пакетов.
  bool route();
  bool send(const QList<quint64> &sockets, const QByteArray &packet, int option = 0, const QByteArray &userId = QByteArray());
  bool send(const QList<quint64> &sockets, const QList<QByteArray> &packets, int option = 0, const QByteArray &userId = QByteArray());
  bool send(const QList<quint64> &sockets, Packet packet, int option = 0, const QByteArray &userId = QByteArray());

  inline NewPacketsEvent *packetsEvent()       { return m_packetsEvent; }
  inline QDataStream *sendStream()             { return m_sendStream; }
  inline void addAuth(NodeAuth *auth)          { m_auth.prepend(auth); }
  QByteArray id() const;
  virtual bool add(ChatChannel channel, int authType, const QByteArray &authId);
  virtual int start() { return 0; }
  virtual void quit() {}

public slots:
  inline void workerReady(QObject *listener) { m_listeners.append(listener); }

protected:
  void customEvent(QEvent *event);

protected:
  virtual bool checkPacket();
  virtual void newPacketsEvent(NewPacketsEvent *event);
  virtual void packet(int type);

  // Авторизация.
  virtual bool auth();
  virtual void accept(const AuthResult &result);
  virtual void reject(const AuthResult &result);

  // users.
  void release(SocketReleaseEvent *event);

  // notices.
  virtual void notice(quint16 type);

  NewPacketsEvent *m_packetsEvent;    ///< Текущий объект NewPacketsEvent.
  Notice *m_notice;                   ///< Текущий прочитанный объект Notice.
  PacketReader *m_reader;             ///< Текущий объект PacketReader выполняющий чтение пакета.
  QByteArray m_readBuffer;            ///< Буфер чтения виртуальных пакетов.
  QByteArray m_sendBuffer;            ///< Буфер отправки виртуальных пакетов.
  QDataStream *m_readStream;          ///< Поток чтения виртуальных пакетов.
  QDataStream *m_sendStream;          ///< Поток отправки виртуальных пакетов.
  qint64 m_timestamp;                 ///< Отметка времени.
  QList<NodeAuth *> m_auth;           ///< Модули авторизации.
  QList<QObject *> m_listeners;       ///< Слушатели событий.
  Settings *m_settings;               ///< Настройки.
  static Core *m_self;                ///< Указатель на себя.
  Storage *m_storage;                 ///< Хранилище данных.
};

#endif /* CORE_H_ */