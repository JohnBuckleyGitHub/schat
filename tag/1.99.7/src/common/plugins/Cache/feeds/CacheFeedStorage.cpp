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

#include <QSqlQuery>
#include <QSqlDatabase>

#include "CacheDB.h"
#include "feeds/CacheFeedStorage.h"
#include "JSON.h"
#include "net/packets/Notice.h"
#include "sglobal.h"

CacheFeedStorage::CacheFeedStorage(QObject *parent)
  : FeedStorage(parent)
{
}


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


void CacheFeedStorage::loadImpl(Channel *channel)
{
  QVariantMap feeds = channel->data()[LS("feeds")].toMap();

  QMapIterator<QString, QVariant> i(feeds);
  while (i.hasNext()) {
    i.next();
    load(channel, i.key(), i.value().toLongLong());
  }
}


qint64 CacheFeedStorage::save(FeedPtr feed, const QByteArray &json)
{
  QSqlQuery query(QSqlDatabase::database(CacheDB::id()));
  query.prepare(LS("INSERT INTO feeds (channel, rev, date, name, json) "
                     "VALUES (:channel, :rev, :date, :name, :json);"));

  query.bindValue(LS(":channel"), feed->head().channel()->key());
  query.bindValue(LS(":rev"),     feed->head().data().value(LS("rev")).toLongLong());
  query.bindValue(LS(":date"),    feed->head().data().value(LS("date")));
  query.bindValue(LS(":name"),    feed->head().name());
  query.bindValue(LS(":json"),    json);
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  return query.lastInsertId().toLongLong();
}


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

  QVariantMap json = JSON::parse(query.value(0).toByteArray()).toMap();

  Feed *feed = FeedStorage::load(name, json);
  channel->feeds().add(feed, false);
}
