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


/*!
 * Получение фида \b hosts.
 *
 * Если фид не существует, то он будет создан, при создании будет задана маска прав доступа 0400.
 */
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


QByteArray Hosts::id(const QByteArray &uniqueId) const
{
  if (uniqueId.isEmpty())
    return currentId();

  return uniqueId;
}


/*!
 * Получение JSON данных хоста.
 *
 * \param uniqueId Публичный идентификатор хоста, если не задан будет использоваться текущий идентификатор.
 *
 * \sa setData().
 * \sa currentId().
 */
QVariantMap Hosts::data(const QByteArray &uniqueId)
{
  return feed()->data().value(SimpleID::encode(id(uniqueId))).toMap();
}


void Hosts::add(const AuthRequest &data, const QString &host)
{
  FeedPtr feed = this->feed();

  QVariantMap json = this->data();
  if (json.isEmpty())
    FeedStorage::clone(feed);

  json[LS("host")]     = host;
  json[LS("os")]       = data.os;
  json[LS("version")]  = Ver(data.version).toString();
  json[LS("offset")]   = data.offset;
  json[LS("name")]     = data.hostName;
  json[LS("date")]     = DateTime::utc();
  json[LS("online")]   = true;

  setData(json);
}


/*!
 * Удаление сокета.
 */
void Hosts::remove(quint64 socket)
{
  if (m_sockets.count(socket) == 1) {
    QByteArray id = m_sockets.publicId(socket);
    if (!id.isEmpty()) {
      QVariantMap json = data(id);
      json[LS("date")]   = DateTime::utc();
      json[LS("online")] = false;
      setData(json, id);
    }
  }

  m_sockets.remove(socket);
}


/*!
 * Установка JSON данных хоста.
 *
 * \param data     Новые данные.
 * \param uniqueId Публичный идентификатор хоста, если не задан будет использоваться текущий идентификатор.
 * \param save     \b true если надо сохранить тело фида.
 */
void Hosts::setData(const QVariantMap &data, const QByteArray &uniqueId, bool save)
{
  FeedPtr feed = this->feed();
  feed->data()[SimpleID::encode(id(uniqueId))] = data;

  if (save)
    FeedStorage::save(feed);
}


int Hosts::Sockets::count(quint64 socket)
{
  if (socket == 0)
    socket = Core::socket();

  if (!m_sockets.contains(socket))
    return 0;

  return m_ids.value(m_sockets.value(socket)).size();
}


QByteArray Hosts::Sockets::publicId(quint64 socket) const
{
  if (socket == 0)
    socket = Core::socket();

  if (!m_sockets.contains(socket))
    return QByteArray();

  return SimpleID::make(Storage::privateId() + m_sockets.value(socket), SimpleID::MessageId);
}


/*!
 * Добавление сокета.
 *
 * \param uniqueId Уникальный идентификатор клиента.
 */
void Hosts::Sockets::add(const QByteArray &uniqueId)
{
  m_sockets.insert(Core::socket(), uniqueId);
  m_ids[uniqueId].append(Core::socket());
}


void Hosts::Sockets::remove(quint64 socket)
{
  if (!m_sockets.contains(socket))
    return;

  QByteArray id = m_sockets.value(socket);
  m_ids[id].removeAll(socket);
  if (m_ids.value(id).isEmpty())
    m_ids.remove(id);

  m_sockets.remove(socket);
}
