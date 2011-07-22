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
class MessageData;
class NoticeData;
class ServerData;
class SimpleClientPrivate;

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

  explicit SimpleClient(QObject *parent = 0);
  ~SimpleClient();

  bool openUrl(const QUrl &url);
  bool send(const MessageData &data);
  bool send(const QByteArray &packet);
  bool send(const QList<QByteArray> &packets);
  Channel* channel(const QByteArray &id) const;
  ClientState clientState() const;
  ClientUser user() const;
  ClientUser user(const QByteArray &id) const;
  inline bool openUrl(const QString &url) { return openUrl(QUrl(url)); }
  QByteArray serverId() const;
  QString nick() const;
  QUrl url() const;
  ServerData *serverData();
  void lock();
  void setNick(const QString &nick);
  void unlock();

  void leave();
  void part(const QByteArray &channelId);

signals:
  void clientStateChanged(int state);
  void join(const QByteArray &channelId, const QByteArray &userId);
  void message(const MessageData &data);
  void notice(const NoticeData &data);
  void part(const QByteArray &channelId, const QByteArray &userId);
  void synced(const QByteArray &channelId);
  void userDataChanged(const QByteArray &userId);
  void userLeave(const QByteArray &userId);

protected:
  SimpleClient(SimpleClientPrivate &dd, QObject * parent);
  void newPacketsImpl();
  void timerEvent(QTimerEvent *event);

private slots:
  void released();
  void requestAuth();

private:
  Q_DECLARE_PRIVATE(SimpleClient);
};

#endif /* SIMPLECLIENT_H_ */
