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
 * \param date Дата.
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
  qint64 key = CacheDB::key(channel->id());
  if (key <= 0)
    return;

  QSqlQuery query(QSqlDatabase::database(CacheDB::id()));
  query.prepare(LS("SELECT name, json FROM feeds WHERE channel = :channel;"));
  query.bindValue(LS(":channel"), key);
  query.exec();

  while (query.next()) {
    QString name = query.value(0).toString();
    Feed *feed = FeedStorage::load(name, JSON::parse(query.value(1).toByteArray()).toMap());
    channel->feeds().add(feed, false);

    ChatNotify::start(new FeedNotify(Notify::FeedData, channel->id(), name));
  }
}
