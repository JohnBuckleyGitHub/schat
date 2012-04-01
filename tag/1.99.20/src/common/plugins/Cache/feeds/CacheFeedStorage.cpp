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
int CacheFeedStorage::saveImpl(FeedPtr feed)
{
  QVariantMap feeds = feed->head().channel()->data().value(LS("feeds")).toMap();
  qint64 id = save(feed, JSON::generate(feed->save()));
  if (id == -1)
    return Notice::InternalError;

  feeds[feed->head().name()] = id;
  feed->head().channel()->data()[LS("feeds")] = feeds;
  CacheDB::saveData(feed->head().channel());

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
 * Запись в базу тела фида.
 */
qint64 CacheFeedStorage::save(FeedPtr feed, const QByteArray &json)
{
  if (feed->head().key() > 0)
    return update(feed, json);

  QSqlQuery query(QSqlDatabase::database(CacheDB::id()));
  query.prepare(LS("INSERT INTO feeds (channel, rev, date, name, json) VALUES (:channel, :rev, :date, :name, :json);"));

  query.bindValue(LS(":channel"), feed->head().channel()->key());
  query.bindValue(LS(":rev"),     feed->head().rev());
  query.bindValue(LS(":date"),    feed->head().date());
  query.bindValue(LS(":name"),    feed->head().name());
  query.bindValue(LS(":json"),    json);
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  qint64 key = query.lastInsertId().toLongLong();
  feed->head().setKey(key);
  return key;
}


qint64 CacheFeedStorage::update(FeedPtr feed, const QByteArray &json)
{
  QSqlQuery query(QSqlDatabase::database(CacheDB::id()));
  query.prepare(LS("UPDATE feeds SET rev = :rev, date = :date, json = :json WHERE id = :id;"));
  query.bindValue(LS(":rev"),  feed->head().rev());
  query.bindValue(LS(":date"), feed->head().date());
  query.bindValue(LS(":json"), json);
  query.bindValue(LS(":id"),   feed->head().key());
  query.exec();

  return feed->head().key();
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

  ChatNotify::start(FeedNotify(Notify::FeedData, channel->id(), name));
}
