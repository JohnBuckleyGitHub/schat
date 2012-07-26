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

#ifndef HOSTS_H_
#define HOSTS_H_

#include <QMap>

#include "feeds/Feed.h"
#include "Host.h"
#include "schat.h"

class AuthRequest;
class ServerChannel;

/*!
 * Обеспечивает средства для работы с хостами пользователя.
 */
class SCHAT_EXPORT Hosts
{
public:
  Hosts(ServerChannel *channel);
  inline QList<quint64> sockets() const          { return m_sockets.keys(); }
  inline void setChannel(ServerChannel *channel) { m_channel = channel; }

  const QHash<QByteArray, HostInfo>& all();
  FeedPtr feed() const;
  FeedPtr user() const;
  QByteArray id(const QByteArray &publicId = QByteArray()) const;
  QList<quint64> sockets(const QByteArray &publicId) const;

  void add(HostInfo hostInfo);
  void remove(quint64 socket);
  void unlink(const QByteArray &hostId);

  static QByteArray toHostId(const QByteArray &uniqueId, const QByteArray &channelId);

private:
  FeedPtr feed(const QString &name, int mask) const;
  QByteArray publicId(quint64 socket = 0) const;
  void updateUser(const QByteArray &publicId, quint64 socket = 0);

  QHash<QByteArray, HostInfo> m_hosts; ///< Таблица хостов, в качестве ключа публичный идентификатор хоста.
  QHash<quint64, HostInfo> m_sockets;  ///< Таблица сокетов и ассоциированных с ними хостов.
  qint64 m_date;                       ///< Последняя дата модификации.
  ServerChannel *m_channel;            ///< Канал.
};

#endif /* HOSTS_H_ */
