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

#include <QDebug>

#include "cores/Core.h"
#include "DateTime.h"
#include "feeds/FeedStorage.h"
#include "Hosts.h"
#include "net/packets/auth.h"
#include "ServerChannel.h"
#include "sglobal.h"
#include "Storage.h"
#include "tools/Ver.h"

Hosts::Hosts()
  : m_channel(0)
{
}


FeedPtr Hosts::feed()
{
  FeedPtr feed = m_channel->feed(LS("hosts"), false);
  if (feed)
    return feed;

  feed = m_channel->feed(LS("hosts"), true, false);
  QVariantMap mask;
  mask[LS("action")] = LS("x-mask");
  mask[LS("mask")] = 0400;
  feed->query(mask, m_channel);
  return feed;
}


/*!
 * Возвращает идентификатор текущего сокета, полученный из приватного идентификатора сервера
 * и уникального идентификатора пользователя.
 */
QByteArray Hosts::currentId()
{
  if (!m_sockets.contains(Core::socket()))
    return QByteArray();

  return SimpleID::make(Storage::privateId() + m_sockets.value(Core::socket()), SimpleID::MessageId);
}


QVariantMap Hosts::data(const QByteArray &uniqueId)
{
  QByteArray id = uniqueId;
  if (uniqueId.isEmpty())
    id = currentId();

  return feed()->data().value(SimpleID::encode(id)).toMap();
}


void Hosts::add(const AuthRequest &data, const QString &host)
{
  qDebug() << m_channel;
  FeedPtr feed = this->feed();
  qDebug() << "##########################" << sockets() << feed;

  QVariantMap json = this->data();
  if (json.isEmpty())
    FeedStorage::clone(feed);

  json[LS("host")]     = host;
  json[LS("os")]       = data.os;
  json[LS("version")]  = Ver(data.version).toString();
  json[LS("offset")]   = data.offset;
  json[LS("name")]     = data.hostName;
  json[LS("date")]     = DateTime::utc();

  setData(json);
}


/*!
 * Добавление сокета.
 *
 * \param uniqueId Уникальный идентификатор клиента.
 */
void Hosts::add(const QByteArray &uniqueId)
{
  m_sockets.insert(Core::socket(), uniqueId);

  qDebug() << "##########################" << sockets();
}


/*!
 * Удаление сокета.
 */
void Hosts::remove(quint64 socket)
{
  m_sockets.remove(socket);
}


void Hosts::setData(const QVariantMap &data, const QByteArray &uniqueId, bool save)
{
  QByteArray id = uniqueId;
  if (uniqueId.isEmpty())
    id = currentId();

  FeedPtr feed = this->feed();
  feed->data()[SimpleID::encode(id)] = data;

  if (save)
    FeedStorage::save(feed);
}
