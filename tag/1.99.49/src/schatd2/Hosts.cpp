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

#include "Ch.h"
#include "cores/Core.h"
#include "DataBase.h"
#include "DateTime.h"
#include "events.h"
#include "feeds/FeedStorage.h"
#include "Hosts.h"
#include "net/packets/auth.h"
#include "net/packets/ChannelNotice.h"
#include "net/packets/FeedNotice.h"
#include "plugins/GeoHook.h"
#include "ServerChannel.h"
#include "sglobal.h"
#include "Sockets.h"
#include "Storage.h"
#include "tools/Ver.h"

Hosts::Hosts(ServerChannel *channel)
  : m_date(0)
  , m_channel(channel)
{
}


const QHash<QByteArray, HostInfo>& Hosts::all()
{
  if (m_hosts.isEmpty())
    m_hosts = DataBase::hosts(m_channel->key());

  return m_hosts;
}


/*!
 * Получение фида \b hosts.
 *
 * Если фид не существует, то он будет создан, при создании будет задана маска прав доступа 0400.
 */
FeedPtr Hosts::feed() const
{
  return feed(LS("hosts"), 0600);
}


FeedPtr Hosts::user() const
{
  return feed(LS("user"), 0444);
}


QByteArray Hosts::id(const QByteArray &publicId) const
{
  if (publicId.isEmpty() || SimpleID::typeOf(publicId) != SimpleID::HostId)
    return this->publicId();

  return publicId;
}


QList<quint64> Hosts::sockets(const QByteArray &publicId) const
{
  if (SimpleID::typeOf(publicId) == SimpleID::HostId) {
    HostInfo host = m_hosts.value(publicId);
    return host->sockets;
  }

  return QList<quint64>();
}


/*!
 * Добавление нового или обновление существующего подключения в списоке хостов.
 */
void Hosts::add(HostInfo hostInfo)
{
  QByteArray id = toHostId(hostInfo->uniqueId, m_channel->id());
  all();

  HostInfo host = m_hosts.value(id);
  if (host) {
    host->name    = hostInfo->name;
    host->address = hostInfo->address;
    host->version = hostInfo->version;
    host->os      = hostInfo->os;
    host->osName  = hostInfo->osName;
    host->tz      = hostInfo->tz;
  }
  else {
    host = hostInfo;
    m_hosts[id] = host;
  }

  host->online  = true;
  host->channel = m_channel->key();
  host->hostId  = id;
  host->geo     = GeoHook::geo(host->address);

  m_date = DateTime::utc();
  host->date = m_date;

  host->sockets.append(hostInfo->socket);
  m_sockets[hostInfo->socket] = host;

  DataBase::add(host);
  FeedStorage::save(feed(), m_date);
  updateUser(QByteArray(), hostInfo->socket);
}


/*!
 * Удаление сокета.
 */
void Hosts::remove(quint64 socket)
{
  if (socket == 0)
    socket = Core::socket();

  HostInfo host = m_sockets.value(socket);
  if (!host)
    return;

  if (host->sockets.size() == 1) {
    host->online = false;
    m_date = DateTime::utc();
    host->date = m_date;

    DataBase::add(host);
    FeedStorage::save(feed(), m_date);
    updateUser(host->hostId);
  }

  host->sockets.removeAll(socket);
  m_sockets.remove(socket);
}


void Hosts::unlink(const QByteArray &hostId)
{
  HostInfo host = m_hosts.value(hostId);
  if (!host)
    return;

  m_hosts.remove(hostId);
  DataBase::removeHost(hostId);

  QList<quint64> sockets = host->sockets;
  if (!sockets.isEmpty()) {
    foreach (quint64 socket, sockets) {
      m_sockets.remove(socket);
    }

    Core::i()->send(sockets, ChannelNotice::request(m_channel->id(), m_channel->id(), LS("quit"))->data(Core::stream()), NewPacketsEvent::KillSocketOption);
  }

  updateUser(hostId);
}


/*!
 * Получение публичного идентификатора хоста на основе приватного идентификатора сервера
 * и уникального идентификатора пользователя.
 *
 * \param uniqueId  Уникальный идентификатор пользователя.
 * \param channelId Идентификатор пользователя.
 */
QByteArray Hosts::toHostId(const QByteArray &uniqueId, const QByteArray &channelId)
{
  return SimpleID::make("host:" + Storage::privateId() + uniqueId + channelId, SimpleID::HostId);
}


FeedPtr Hosts::feed(const QString &name, int mask) const
{
  FeedPtr feed = m_channel->feed(name, false);
  if (!feed) {
    feed = m_channel->feed(name, true, false);
    feed->head().acl().add(m_channel->id());
  }

  feed->head().acl().setMask(mask);
  return feed;
}


QByteArray Hosts::publicId(quint64 socket) const
{
  if (socket == 0)
    socket = Core::socket();

  if (!m_sockets.contains(socket))
    return QByteArray();

  return m_sockets.value(socket)->hostId;
}


/*!
 * Обновление фида \b user при отключении или подключении пользователя.
 */
void Hosts::updateUser(const QByteArray &publicId, quint64 socket)
{
  FeedPtr feed = user();
  FeedStorage::save(feed, m_date);

  QList<quint64> sockets = Sockets::all(Ch::channel(m_channel->id()), true);
  if (publicId.isEmpty())
    sockets.removeAll(socket);

  if (sockets.isEmpty())
    return;

  QVariantMap headers = Feed::merge(LS("f"), feed->head().f());
  if (headers.isEmpty())
    return;

  FeedNotice packet(m_channel->id(), m_channel->id(), LS("get"));
  packet.setText(LS("*"));
  packet.setData(headers);
  Core::i()->send(sockets, packet.data(Core::stream()));
}
