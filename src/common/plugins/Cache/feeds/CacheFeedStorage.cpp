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

#include <QSqlQuery>
#include <QSqlDatabase>

#include "CacheDB.h"
#include "ChatNotify.h"
#include "feeds/CacheFeedStorage.h"
#include "JSON.h"
#include "net/packets/Notice.h"
#include "sglobal.h"

CacheFeedStorage::CacheFeedStorage(QObject *parent)
  : FeedStorage(parent)
{
}


/*!
 * Реализация сохранения фида.
 *
 * \param feed Фид.
 */
int CacheFeedStorage::saveImpl(FeedPtr feed, qint64 /*date*/)
{
  CacheDB::add(feed);
  return Notice::OK;
}


/*!
 * Реализация загрузки фида.
 */
void CacheFeedStorage::loadImpl(Channel *channel)
{
  QVariantMap feeds = channel->data()[LS("feeds")].toMap();

  QMapIterator<QString, QVariant> i(feeds);
  while (i.hasNext()) {
    i.next();
    load(channel, i.key(), i.value().toLongLong());
  }
}


/*!
 * Загрузка фида из базы данных.
 *
 * \param channel Канал.
 * \param name    Имя фида.
 * \param id      Ключ в таблице фидов.
 */
void CacheFeedStorage::load(Channel *channel, const QString &name, qint64 id)
{
  if (id <= 0)
    return;

  QSqlQuery query(QSqlDatabase::database(CacheDB::id()));
  query.prepare(LS("SELECT json FROM feeds WHERE id = :id LIMIT 1;"));
  query.bindValue(LS(":id"), id);
  query.exec();

  if (!query.first())
    return;

  Feed *feed = FeedStorage::load(name, JSON::parse(query.value(0).toByteArray()).toMap());
  feed->head().setKey(id);
  channel->feeds().add(feed, false);

  ChatNotify::start(new FeedNotify(Notify::FeedData, channel->id(), name));
}
