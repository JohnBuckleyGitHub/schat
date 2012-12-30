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
#include "Channel.h"
#include "DateTime.h"
#include "feeds/NodeInfoFeed.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "Storage.h"

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
  qint64 date = DateTime::utc();

  // Установка текстового заголовка канала.
  if (path == LS("title")) {
    QVariantMap data;
    data[LS("text")]   = value.toString().left(200);
    data[LS("author")] = SimpleID::encode(channel->id());
    data[LS("date")]   = date;

    m_data[path] = data;
    return FeedReply(Notice::OK, date);
  }
  else if (path == LS("visibility")) {
    visibility(value.toInt());
    return FeedReply(Notice::OK, date);
  }
  else if (path == LS("pinned")) {
    if (!Ch::server()->feed(LS("acl"))->can(channel, Acl::Edit))
      return Notice::Forbidden;

    m_data[path] = value.toBool();
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
  return Notice::Forbidden;
}


/*!
 * Обновление настроек видимости канала.
 *
 * При необходимости обновляется опция "PermanentChannels" в хранилище, для обновления списка постоянных каналов.
 */
void NodeInfoFeed::visibility(int value)
{
  const QString key     = LS("PermanentChannels");
  const QString id      = SimpleID::encode(m_header.channel()->id());
  QStringList permanent = Storage::value(key).toStringList();
  bool contains         = permanent.contains(id);

  if (value > 0) {
    if (!contains) {
      permanent.append(id);
      Storage::setValue(key, permanent);
    }
  }
  else if (contains) {
    permanent.removeAll(id);
    Storage::setValue(key, permanent);
  }

  static_cast<ServerChannel *>(m_header.channel())->setPermanent(value > 0);
  m_data[LS("visibility")] = value;
}
