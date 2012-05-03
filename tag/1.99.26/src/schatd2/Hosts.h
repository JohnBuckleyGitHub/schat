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

/*!
 * Обеспечивает средства для работы с хостами пользователя.
 */
class SCHAT_EXPORT Hosts
{
public:
  Hosts();
  inline void setChannel(ServerChannel *channel) { m_channel = channel; }

  FeedPtr feed() const;
  FeedPtr user() const;
  QByteArray currentId() const;
  QByteArray id(const QByteArray &publicId) const;
  QList<quint64> sockets() const;
  QList<quint64> sockets(const QByteArray &publicId) const;
  QVariantMap data(const QByteArray &publicId = QByteArray()) const;
  QVariantMap userData(const QByteArray &publicId = QByteArray()) const;

  void add(const AuthRequest &data, const QString &host);
  void add(const QByteArray &uniqueId);
  void remove(quint64 socket);
  void setData(const QVariantMap &data, const QByteArray &publicId = QByteArray(), bool save = true);
  void setUserData(const QVariantMap &data, const QByteArray &publicId = QByteArray(), bool save = true);

  static QByteArray toPublicId(const QByteArray &uniqueId);

private:
  FeedPtr feed(const QString &name, int mask) const;

  class Sockets
  {
  public:
    Sockets() {}
    inline const QMap<QByteArray, QList<quint64> >& ids() const { return m_ids; }
    inline const QMap<quint64, QByteArray>& sockets() const     { return m_sockets; }
    inline QList<quint64> socketsList() const                   { return m_sockets.keys(); }
    int count(quint64 socket = 0);
    QByteArray publicId(quint64 socket = 0) const;

    void add(const QByteArray &publicId);
    void remove(quint64 socket);

  private:
    QMap<quint64, QByteArray> m_sockets;     ///< Таблица сокетов и уникальных идентификаторов пользователя.
    QMap<QByteArray, QList<quint64> > m_ids; ///< Обратная таблица.
  };

  ServerChannel *m_channel;                  ///< Канал.
  Sockets m_sockets;                         ///< Сокеты.
};

#endif /* HOSTS_H_ */
