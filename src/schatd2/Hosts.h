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

#include "schat.h"
#include "feeds/Feed.h"

class AuthRequest;
class ServerChannel;

class SCHAT_EXPORT Hosts
{
public:
  Hosts();
  FeedPtr feed();
  inline QList<quint64> sockets() const          { return m_sockets.keys(); }
  inline void setChannel(ServerChannel *channel) { m_channel = channel; }
  QByteArray currentId();
  QVariantMap data(const QByteArray &uniqueId = QByteArray());
  void add(const AuthRequest &data, const QString &host);
  void add(const QByteArray &uniqueId);
  void remove(quint64 socket);
  void setData(const QVariantMap &data, const QByteArray &uniqueId = QByteArray(), bool save = true);

private:
  QMap<quint64, QByteArray> m_sockets; ///< Таблица сокетов и уникальных идентификаторов пользователя.
  ServerChannel *m_channel;            ///< Канал.
};

#endif /* HOSTS_H_ */
