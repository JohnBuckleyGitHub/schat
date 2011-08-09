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
#include <QHostAddress>
#include <QList>

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

  ServerEvent(ServerEvents type, const QList<quint64> &sockets, const QByteArray &userId = QByteArray());
  ServerEvent(ServerEvents type, quint64 socket, const QByteArray &userId = QByteArray());
  inline QByteArray userId() const { return m_userId; }
  inline QList<quint64> sockets() const { return m_sockets; }
  quint64 socket() const;

protected:
  QByteArray m_userId;      ///< Идентификатор пользователя.
  QList<quint64> m_sockets; ///< Идентификаторы сокетов.
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
  NewPacketsEvent(quint64 socket, const QByteArray &packet, const QByteArray &userId = QByteArray());
  NewPacketsEvent(quint64 socket, const QList<QByteArray> &packets, const QByteArray &userId = QByteArray());

  const QList<QByteArray> packets; ///< Тела виртуальных пакетов.
  int option;                      ///< Опция /sa Option.
  QHostAddress address;            ///< Адрес сокета.
  qint64 timestamp;                ///< Отметка времени.
};


class SocketReleaseEvent : public ServerEvent
{
public:
  SocketReleaseEvent(quint64 sockets, const QString &errorString, const QByteArray &userId = QByteArray());

  const QString errorString;
};

#endif /* EVENTS_H_ */
