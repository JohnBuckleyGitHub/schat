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

#include "events.h"

ServerEvent::ServerEvent(ServerEvents type, const QList<quint64> &sockets, const QByteArray &userId)
  : QEvent(static_cast<QEvent::Type>(type))
  , m_userId(userId)
  , m_sockets(sockets)
{
}


ServerEvent::ServerEvent(ServerEvents type, quint64 socket, const QByteArray &userId)
  : QEvent(static_cast<QEvent::Type>(type))
  , m_userId(userId)
{
  m_sockets.append(socket);
}


/*!
 * Получение первого сокета в списке.
 */
quint64 ServerEvent::socket() const
{
  if (m_sockets.isEmpty())
    return 0;

  return m_sockets.at(0);
}


NewPacketsEvent::NewPacketsEvent(const QList<quint64> &sockets, const QByteArray &packet)
  : ServerEvent(NewPackets, sockets)
  , packets(QList<QByteArray>() << packet)
  , option(0)
  , timestamp(0)
{
}


NewPacketsEvent::NewPacketsEvent(const QList<quint64> &sockets, const QList<QByteArray> &packets)
  : ServerEvent(NewPackets, sockets)
  , packets(packets)
  , option(0)
  , timestamp(0)
{
}


NewPacketsEvent::NewPacketsEvent(quint64 socket, const QByteArray &packet, const QByteArray &userId)
  : ServerEvent(NewPackets, socket, userId)
  , packets(QList<QByteArray>() << packet)
  , option(0)
  , timestamp(0)
{
}


NewPacketsEvent::NewPacketsEvent(quint64 socket, const QList<QByteArray> &packets, const QByteArray &userId)
  : ServerEvent(NewPackets, socket, userId)
  , packets(packets)
  , option(0)
  , timestamp(0)
{
}


SocketReleaseEvent::SocketReleaseEvent(quint64 socket, const QString &errorString, const QByteArray &userId)
  : ServerEvent(SocketRelease, socket, userId)
  , errorString(errorString)
{
}
