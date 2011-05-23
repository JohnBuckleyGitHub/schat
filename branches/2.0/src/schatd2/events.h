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

#ifndef EVENTS_H_
#define EVENTS_H_

#include <QEvent>
#include <QList>
#include <QString>

/*!
 * Базовый класс для событий сервера.
 */
class ServerEvent : public QEvent
{
public:
  enum ServerEvents {
    NewPackets = 7666,
    SocketRelease = 7667
  };

  ServerEvent(ServerEvents type, int workerId, quint64 socketId, const QByteArray &clientId = QByteArray());
  inline int workerId() const { return m_workerId; }
  inline QByteArray userId() const { return m_userId; }
  inline quint64 socketId() const { return m_socketId; }

private:
  const int m_workerId;        /// Идентификатор объекта Worker, к которому принадлежит сокет.
  const QByteArray m_userId;   /// Идентификатор пользователя.
  const quint64 m_socketId;    /// Идентификатор сокета.
};


/*!
 * Отправка пакетов между одиночным сокетом и ядром чата и наоборот.
 */
class NewPacketsEvent : public ServerEvent
{
public:
  enum Option {
    AuthorizeSocketOption = 1,
    KillSocketOption
  };

  NewPacketsEvent(const QList<quint64> &socketIds, const QByteArray &packet);
  NewPacketsEvent(const QList<quint64> &socketIds, const QList<QByteArray> &packets);
  NewPacketsEvent(int workerId, quint64 socketId, const QByteArray &packet, const QByteArray &clientId = QByteArray());
  NewPacketsEvent(int workerId, quint64 socketId, const QList<QByteArray> &packets, const QByteArray &clientId = QByteArray());

  const QList<QByteArray> packets;
  int option;
  qint64 timestamp;
  QList<quint64> socketIds;
};


class SocketReleaseEvent : public ServerEvent
{
public:
  SocketReleaseEvent(int workerId, quint64 socketId, const QString &errorString, const QByteArray &clientId = QByteArray());

  const QString errorString;
};

#endif /* EVENTS_H_ */
