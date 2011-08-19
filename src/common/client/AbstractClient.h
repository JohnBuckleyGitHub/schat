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

#ifndef ABSTRACTCLIENT_H_
#define ABSTRACTCLIENT_H_

#include <QUrl>

#include "Channel.h"
#include "net/SimpleSocket.h"
#include "schat.h"
#include "User.h"

class AbstractClientPrivate;
class Channel;
class MessageData;
class NoticeData;
class PacketReader;
class ServerData;

class SCHAT_EXPORT AbstractClient : public SimpleSocket
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

  explicit AbstractClient(QObject *parent = 0);
  ~AbstractClient();

  bool openUrl(const QUrl &url);
  bool send(const MessageData &data);
  bool send(const QByteArray &packet);
  bool send(const QList<QByteArray> &packets);
  ClientChannel channel(const QByteArray &id) const;
  ClientState clientState() const;
  ClientUser user() const;
  ClientUser user(const QByteArray &id) const;
  inline bool openUrl(const QString &url) { return openUrl(QUrl(url)); }
  PacketReader *reader();
  QByteArray serverId() const;
  QByteArray uniqueId() const;
  QString nick() const;
  QUrl url() const;
  ServerData *serverData();
  void lock();
  void setNick(const QString &nick);
  void unlock();

  void leave();
  void part(const QByteArray &channelId);

signals:
  void clientStateChanged(int state, int previousState);
  void join(const QByteArray &channelId, const QByteArray &userId);
  void message(const MessageData &data);
  void notice(const NoticeData &data);
  void packetReady(int type);
  void part(const QByteArray &channelId, const QByteArray &userId);
  void ready();
  void requestClientAuth();
  void synced(const QByteArray &channelId);
  void userDataChanged(const QByteArray &userId);
  void userLeave(const QByteArray &userId);

protected:
  AbstractClient(AbstractClientPrivate &dd, QObject *parent);
  void newPacketsImpl();
  void timerEvent(QTimerEvent *event);

protected slots:
  void released();
  virtual void requestAuth();

private:
  Q_DECLARE_PRIVATE(AbstractClient);
};

#endif /* ABSTRACTCLIENT_H_ */
