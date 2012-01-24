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

#include "Ch.h"
#include "DateTime.h"
#include "feeds/FeedStorage.h"
#include "feeds/NodeLinksFeed.h"
#include "net/packets/auth.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "Storage.h"
#include "tools/Ver.h"

NodeLinksFeed::NodeLinksFeed(const QString &name, const QVariantMap &data)
  : Feed(name, data)
{
  m_header.acl().setMask(0600);
}


NodeLinksFeed::NodeLinksFeed(const QString &name, qint64 date)
  : Feed(name, date)
{
  m_header.acl().setMask(0600);
}


Feed* NodeLinksFeed::create(const QString &name)
{
  return new NodeLinksFeed(name, DateTime::utc());
}


Feed* NodeLinksFeed::load(const QString &name, const QVariantMap &data)
{
  return new NodeLinksFeed(name, data);
}


/*!
 * Обработка запросов.
 */
FeedQueryReply NodeLinksFeed::query(const QVariantMap &json, Channel *channel)
{
  QString action = json.value(LS("action")).toString();
  if (action.isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  if (action == LS("add"))
    return add(json, channel);

  return FeedQueryReply(Notice::ServiceUnavailable);
}


/*!
 * Формирование тела фида для отправки клиенту.
 * Переопределение этой функции необходимо для того чтобы
 * не передавать уникальный идентификатор пользователя.
 */
QVariantMap NodeLinksFeed::feed(Channel *channel)
{
  QVariantMap header = head().get(channel);
  if (header.isEmpty())
    return QVariantMap();

  QVariantMap out;

  QMapIterator<QString, QVariant> i(m_data);
  while (i.hasNext()) {
    i.next();
    if (i.value().type() != QVariant::Map)
      continue;

    QVariantMap item = i.value().toMap();
    QString id = item.value(LS("id")).toString();
    if (id.isEmpty())
      continue;

    item.remove(LS("id"));
    out[id] = item;
  }

  merge(LS("head"), out, header);
  return out;
}


/*!
 * Обработка подключения пользователя.
 */
void NodeLinksFeed::add(ChatChannel channel, const AuthRequest &data, const QString &host)
{
  FeedPtr feed = channel->feed(LS("links"), false);
  if (!feed)
    return;

  QVariantMap query;
  query[LS("action")]   = LS("add");
  query[LS("host")]     = host;
  query[LS("uniqueId")] = SimpleID::encode(data.uniqueId);
  query[LS("os")]       = data.os;
  query[LS("version")]  = Ver(data.version).toString();
  query[LS("offset")]   = data.offset;
  query[LS("name")]     = data.hostName;
  query[LS("date")]     = DateTime::utc();

  if (feed->query(query, Ch::server().data()).status == Notice::OK)
    FeedStorage::save(feed);
}


/*!
 * Обработка "add" запроса.
 *
 * \param json    тело запроса.
 * \param channel канал, только канал-сервер имеет права на выполнение.
 */
FeedQueryReply NodeLinksFeed::add(const QVariantMap &json, Channel *channel)
{
  if (!channel || channel->type() != SimpleID::ServerId)
    return FeedQueryReply(Notice::Forbidden);

  QString uniqueId = json.value(LS("uniqueId")).toString();
  QVariantMap data = m_data.value(uniqueId).toMap();

  merge(data, json);
  data.remove(LS("action"));
  data.remove(LS("uniqueId"));

  if (!data.contains(LS("id")))
    data[LS("id")] = SimpleID::encode(SimpleID::randomId(SimpleID::MessageId, Storage::privateId()));

  m_data[uniqueId] = data;

  return FeedQueryReply(Notice::OK);
}
