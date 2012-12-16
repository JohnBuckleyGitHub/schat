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

#include "Channel.h"
#include "DateTime.h"
#include "feeds/NodeInfoFeed.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "sglobal.h"

NodeInfoFeed::NodeInfoFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  m_header.acl().setMask(0444);
}


NodeInfoFeed::NodeInfoFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  m_header.acl().setMask(0444);
}


Feed* NodeInfoFeed::create(const QString &name)
{
  return new NodeInfoFeed(name, DateTime::utc());
}


Feed* NodeInfoFeed::load(const QString &name, const QVariantMap &data)
{
  return new NodeInfoFeed(name, data);
}


/*!
 * Переопределение проверки прав доступа.
 *
 * Этот фид использует права доступа фида "acl".
 */
bool NodeInfoFeed::can(Channel *channel, Acl::ResultAcl acl) const
{
  if (!channel && acl != Acl::Read)
    return false;

  FeedPtr feed = channel->feed(LS("acl"), false, false);
  if (feed)
    return feed->can(channel, acl);

  return Feed::can(channel, acl);
}


/*!
 * Переопределение запроса "delete".
 *
 * Для этой операции требуются права на редактирование.
 */
FeedReply NodeInfoFeed::del(const QString &path, Channel *channel)
{
  if (!can(channel, Acl::Edit))
    return Notice::Forbidden;

  return Feed::del(path, channel);
}


FeedReply NodeInfoFeed::post(const QString &path, const QVariantMap &json, Channel *channel)
{
  if (!can(channel, Acl::Edit))
    return Notice::Forbidden;

  if (!channel || path.isEmpty() || path.contains(LC('*')) || !json.contains(LS("value")))
    return Notice::BadRequest;

  const QVariant& value = json[LS("value")];

  // Установка текстового заголовка канала.
  if (path == LS("title")) {
    qint64 date = DateTime::utc();

    QVariantMap data;
    data[LS("text")]   = value.toString().left(512);
    data[LS("author")] = SimpleID::encode(channel->id());
    data[LS("date")]   = date;

    m_data[path] = data;
    return FeedReply(Notice::OK, date);
  }

  return Notice::BadRequest;
}


/*!
 * Переопределение запроса "put".
 *
 * Этот фид не поддерживает такой запрос.
 */
FeedReply NodeInfoFeed::put(const QString &path, const QVariantMap &json, Channel *channel)
{
  Q_UNUSED(path)
  Q_UNUSED(json)
  Q_UNUSED(channel)
  return Notice::BadRequest;
}
