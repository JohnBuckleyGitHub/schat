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

#include "DataBase.h"
#include "feeds/NodeAccountFeed.h"
#include "feeds/NodeAclFeed.h"
#include "feeds/NodeFeedStorage.h"
#include "JSON.h"
#include "net/packets/Notice.h"
#include "NodeTopicFeed.h"
#include "ServerChannel.h"

NodeFeedStorage::NodeFeedStorage(QObject *parent)
  : FeedStorage(parent)
{
  add(new NodeAccountFeed());
  add(new NodeAclFeed());
  add(new NodeTopicFeed());

  start();
}


int NodeFeedStorage::revertImpl(FeedPtr feed, const QVariantMap &data)
{
  qint64 rev = feed->head().data().value("rev").toLongLong();
  if (rev == 1)
    return Notice::InternalError;

  qint64 revTo = data.value("rev").toLongLong();
  if (revTo == 0)
    revTo = rev - 1;

  QSqlQuery query;
  query.prepare("SELECT rev, date, json FROM feeds WHERE channel = :channel AND rev = :rev AND name = :name LIMIT 1;");
  query.bindValue(":channel", feed->head().channel()->key());
  query.bindValue(":rev",     revTo);
  query.bindValue(":name",    feed->head().name());
  query.exec();

  if (!query.first())
    return Notice::InternalError;

  QByteArray body = query.value(2).toByteArray();
  QVariantMap json = JSON::parse(body).toMap();

  Feed *f = FeedStorage::load(feed->head().name(), json);
  f->head().data()["rev"]  = rev;
  f->head().data()["size"] = body.size();

  feed->head().channel()->feeds().add(f);
  return Notice::OK;
}


/*!
 * Реализация сохранения фида.
 */
int NodeFeedStorage::saveImpl(FeedPtr feed)
{
  qint64 rev = feed->head().data().value("rev").toLongLong();

  feed->head().data().remove("rev");
  feed->head().data().remove("size");

  QByteArray json = JSON::generate(feed->save());
  QVariantMap feeds = feed->head().channel()->data().value("feeds").toMap();

  // Специальная обработка получения ревизии для случая если фид был раннее существовал но был удалён.
  if (feeds.contains(feed->head().name())) {
    qint64 id = feeds.value(feed->head().name()).toLongLong();
    if (id < 0)
      rev = -id;
  }

  rev++;

  feed->head().data()["rev"]  = rev;
  feed->head().data()["size"] = json.size();

  qint64 id = save(feed, json);
  if (id == -1)
    return Notice::InternalError;


  feeds[feed->head().name()] = id;
  feed->head().channel()->data()["feeds"] = feeds;
  DataBase::saveData(feed->head().channel());

  return Notice::OK;
}


void NodeFeedStorage::loadImpl(Channel *channel)
{
  QVariantMap feeds = channel->data()["feeds"].toMap();

  QMapIterator<QString, QVariant> i(feeds);
  while (i.hasNext()) {
    i.next();
    load(channel, i.key(), i.value().toLongLong());
  }
}


void NodeFeedStorage::removeImpl(FeedPtr feed)
{
  QVariantMap feeds = feed->head().channel()->data()["feeds"].toMap();
  feeds[feed->head().name()] = -feed->head().data()["rev"].toLongLong();
  feed->head().channel()->data()["feeds"] = feeds;
  DataBase::saveData(feed->head().channel());
}


/*!
 * Запись в базу новой ревизии фида.
 */
qint64 NodeFeedStorage::save(FeedPtr feed, const QByteArray &json)
{
  QSqlQuery query;
  query.prepare("INSERT INTO feeds (channel, rev, date, name, json) "
                     "VALUES (:channel, :rev, :date, :name, :json);");

  query.bindValue(":channel", feed->head().channel()->key());
  query.bindValue(":rev",     feed->head().data().value("rev").toLongLong());
  query.bindValue(":date",    feed->head().data().value("date"));
  query.bindValue(":name",    feed->head().name());
  query.bindValue(":json",    json);
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  return query.lastInsertId().toLongLong();
}


void NodeFeedStorage::load(Channel *channel, const QString &name, qint64 id)
{
  if (id <= 0)
    return;

  QSqlQuery query;
  query.prepare("SELECT rev, date, json FROM feeds WHERE id = :id LIMIT 1;");
  query.bindValue(":id", id);
  query.exec();

  if (!query.first())
    return;

  QByteArray data = query.value(2).toByteArray();
  QVariantMap json = JSON::parse(data).toMap();

  Feed *feed = FeedStorage::load(name, json);
  feed->head().data()["rev"]  = query.value(0).toLongLong();
  feed->head().data()["size"] = data.size();

  channel->feeds().add(feed, false);
}


void NodeFeedStorage::start()
{
  QSqlQuery query;

  query.exec(
  "CREATE TABLE IF NOT EXISTS feeds ( "
  "  id         INTEGER PRIMARY KEY,"
  "  channel    INTEGER DEFAULT ( 0 ),"
  "  rev        INTEGER DEFAULT ( 0 ),"
  "  date       INTEGER DEFAULT ( 0 ),"
  "  name       TEXT    NOT NULL,"
  "  json       BLOB"
  ");");
}
