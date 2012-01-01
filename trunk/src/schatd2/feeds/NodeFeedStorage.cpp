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

#include "feeds/NodeFeedStorage.h"
#include "JSON.h"
#include "net/packets/Notice.h"
#include "ServerChannel.h"

NodeFeedStorage::NodeFeedStorage(QObject *parent)
  : FeedStorage(parent)
{
  start();
}


/*!
 * Реализация сохранения фида.
 */
int NodeFeedStorage::saveImpl(FeedPtr feed)
{
  qDebug() << " - - - saveImpl()" << feed->h().name();
  qint64 rev = feed->h().data().value("rev").toLongLong();
  rev++;

  feed->h().data().remove("rev");
  feed->h().data().remove("size");

  QByteArray json = JSON::generate(feed->save());

  feed->h().data()["rev"]  = rev;
  feed->h().data()["size"] = json.size();

  qint64 id = save(feed, json);
  if (id == -1)
    return Notice::InternalError;

  return Notice::OK;
}


/*!
 * Запись в базу новой ревизии фида.
 */
qint64 NodeFeedStorage::save(FeedPtr feed, const QByteArray &json)
{
  QSqlQuery query;
  query.prepare("INSERT INTO feeds (channel, rev, date, name, json) "
                     "VALUES (:channel, :rev, :date, :name, :json);");

  query.bindValue(":channel", static_cast<ServerChannel *>(feed->h().channel())->key());
  query.bindValue(":rev",     feed->h().data().value("rev").toLongLong());
  query.bindValue(":date",    feed->h().data().value("date"));
  query.bindValue(":name",    feed->h().name());
  query.bindValue(":json",    json);
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  return query.lastInsertId().toLongLong();
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
