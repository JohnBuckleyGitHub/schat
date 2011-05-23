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

ServerEvent::ServerEvent(ServerEvents type, int workerId, quint64 socketId, const QByteArray &clientId)
  : QEvent(static_cast<QEvent::Type>(type))
  , m_workerId(workerId)
  , m_userId(clientId)
  , m_socketId(socketId)
{
}


NewPacketsEvent::NewPacketsEvent(const QList<quint64> &socketIds, const QByteArray &packet)
  : ServerEvent(NewPackets, -1, 0, QByteArray())
  , packets(QList<QByteArray>() << packet)
  , option(0)
  , timestamp(0)
  , socketIds(socketIds)
{
}


NewPacketsEvent::NewPacketsEvent(const QList<quint64> &socketIds, const QList<QByteArray> &packets)
  : ServerEvent(NewPackets, -1, 0, QByteArray())
  , packets(packets)
  , option(0)
  , timestamp(0)
  , socketIds(socketIds)
{
}


NewPacketsEvent::NewPacketsEvent(int workerId, quint64 socketId, const QByteArray &packet, const QByteArray &clientId)
  : ServerEvent(NewPackets, workerId, socketId, clientId)
  , packets(QList<QByteArray>() << packet)
  , option(0)
  , timestamp(0)
{
}


NewPacketsEvent::NewPacketsEvent(int workerId, quint64 socketId, const QList<QByteArray> &packets, const QByteArray &clientId)
  : ServerEvent(NewPackets, workerId, socketId, clientId)
  , packets(packets)
  , option(0)
  , timestamp(0)
{
}


SocketReleaseEvent::SocketReleaseEvent(int workerId, quint64 socketId, const QString &errorString, const QByteArray &clientId)
  : ServerEvent(SocketRelease, workerId, socketId, clientId),
    errorString(errorString)
{
}
