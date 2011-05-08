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

#include <QObject>

class MessageData;
class NewPacketsEvent;
class PacketReader;
class ServerChannel;
class ServerUser;
class SocketReleaseEvent;
class Storage;
class Worker;
class WorkerThread;

class Core : public QObject
{
  Q_OBJECT

public:
  Core(QObject *parent = 0);
  ~Core();
  int start();
  void quit();

protected:
  void customEvent(QEvent *event);

private slots:
  void workersStarted();

private:
  bool route();
  bool route(ServerChannel *channel);
  bool route(ServerUser *user);
  bool send(const QList<quint64> &sockets, const QList<QByteArray> &packets);
  bool send(ServerChannel *channel, const QByteArray &packet);
  bool send(ServerChannel *channel, const QList<QByteArray> &packets);
  bool send(ServerUser *user, const QByteArray &packet, int option = 0);
  bool send(ServerUser *user, const QList<QByteArray> &packets, int option = 0);

  void newPacketsEvent(NewPacketsEvent *event);
  void socketReleaseEvent(SocketReleaseEvent *event);

  bool join(const QByteArray &userId, const QByteArray &channelId);
  bool join(const QByteArray &userId, ServerChannel *channel);
  ServerChannel *channel(const QString &name, bool create = true);

  bool command();
  bool readAuthRequest();
  bool readJoinCmd();
  bool readMessage();
  int auth();
  void sendChannel(ServerChannel *channel, ServerUser *user);

  MessageData *m_messageData;      ///< Текущий прочитанный объект MessageData.
  NewPacketsEvent *m_packetsEvent; ///< Текущий объект NewPacketsEvent.
  PacketReader *m_reader;          ///< Текущий объект PacketReader выполняющий чтение пакета.
  QByteArray m_readBuffer;         ///< Буфер чтения виртуальных пакетов.
  QByteArray m_sendBuffer;         ///< Буфер отправки виртуальных пакетов.
  QDataStream *m_readStream;       ///< Поток чтения виртуальных пакетов.
  QDataStream *m_sendStream;       ///< Поток отправки виртуальных пакетов.
  QList<Worker*> m_workers;        ///< Список объектов Worker.
  Storage *m_storage;              ///< Хранилище данных.
  WorkerThread *m_thread;          ///< Поток обслуживающий подключения.

};

#endif /* CORE_H_ */
