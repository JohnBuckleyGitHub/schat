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

#include <QSqlDatabase>
#include <QSqlQuery>

#include "CacheDB.h"
#include "feeds/FeedStorage.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "sglobal.h"

QString CacheDB::m_id;


/*!
 * Открытие базы данных.
 */
bool CacheDB::open(const QByteArray &id, const QString &dir)
{
  QString newId = SimpleID::encode(id) + LS("-cache");

  if (!m_id.isEmpty() && m_id == newId)
      return false;

  close();
  m_id = newId;

  QSqlDatabase db = QSqlDatabase::addDatabase(LS("QSQLITE"), m_id);
  db.setDatabaseName(dir + LS("/cache.sqlite"));
  if (!db.open())
    return false;

  create();
  return true;
}


ClientChannel CacheDB::channel(const QByteArray &id, bool feeds)
{
  qint64 key = channelKey(id, SimpleID::typeOf(id));
  if (key == -1)
    return ClientChannel();

  return channel(key, feeds);
}


ClientChannel CacheDB::channel(qint64 id, bool feeds)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare("SELECT channel, gender, name, data FROM channels WHERE id = :id LIMIT 1;");
  query.bindValue(":id", id);
  query.exec();

  if (!query.first())
    return ClientChannel();

  ClientChannel channel(new Channel(query.value(0).toByteArray(), query.value(2).toString()));
  channel->setKey(id);
  channel->gender().setRaw(query.value(1).toLongLong());
  channel->setData(JSON::parse(query.value(3).toByteArray()).toMap());

  if (feeds)
    FeedStorage::load(channel.data());

  return channel;
}


/*!
 * Добавление или обновление канала.
 *
 * \return Ключ в таблице \b channels.
 */
qint64 CacheDB::add(ClientChannel channel)
{
  if (channel->key() <= 0)
    channel->setKey(channelKey(channel->id(), channel->type()));

  if (channel->key() > 0) {
    update(channel);
    return channel->key();
  }

  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare("INSERT INTO channels (channel, type, gender, name, data) "
                     "VALUES (:channel, :type, :gender, :name, :data);");

  query.bindValue(":channel",    channel->id());
  query.bindValue(":type",       channel->type());
  query.bindValue(":gender",     channel->gender().raw());
  query.bindValue(":name",       channel->name());
  query.bindValue(":data",       JSON::generate(channel->data()));
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  channel->setKey(query.lastInsertId().toLongLong());
  return channel->key();
}


/*!
 * Возвращает ключ в таблице \b channels на основе идентификатора канала и типа канала.
 */
qint64 CacheDB::channelKey(const QByteArray &id, int type)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare("SELECT id FROM channels WHERE channel = :id AND type = :type LIMIT 1;");
  query.bindValue(":id",   id);
  query.bindValue(":type", type);
  query.exec();

  if (!query.first())
    return -1;

  return query.value(0).toLongLong();
}


void CacheDB::clear()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec("DROP TABLE IF EXISTS channels;");
  query.exec("DROP TABLE IF EXISTS feeds");
  query.exec("VACUUM;");

  create();
}


/*!
 * Закрытие базы данных.
 */
void CacheDB::close()
{
  QSqlDatabase::removeDatabase(m_id);
  m_id.clear();
}


void CacheDB::create()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec(LS("PRAGMA synchronous = OFF"));

  query.exec(LS(
    "CREATE TABLE IF NOT EXISTS channels ( "
    "  id         INTEGER PRIMARY KEY,"
    "  channel    BLOB    NOT NULL UNIQUE,"
    "  type       INTEGER DEFAULT ( 73 ),"
    "  gender     INTEGER DEFAULT ( 0 ),"
    "  name       TEXT    NOT NULL,"
    "  data       BLOB"
    ");"
  ));

  query.exec(LS(
    "CREATE TABLE IF NOT EXISTS feeds ( "
    "  id         INTEGER PRIMARY KEY,"
    "  channel    INTEGER DEFAULT ( 0 ),"
    "  rev        INTEGER DEFAULT ( 0 ),"
    "  date       INTEGER DEFAULT ( 0 ),"
    "  name       TEXT    NOT NULL,"
    "  json       BLOB"
    ");"
  ));
}


void CacheDB::saveData(Channel *channel)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(LS("UPDATE channels SET data = :data WHERE id = :id;"));
  query.bindValue(LS(":data"),       JSON::generate(channel->data()));
  query.bindValue(LS(":id"),         channel->key());
  query.exec();
}


/*!
 * Обновление информации о канале.
 */
void CacheDB::update(ClientChannel channel)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare("UPDATE channels SET channel = :channel, type = :type, gender = :gender, name = :name, data = :data WHERE id = :id;");
  query.bindValue(":channel",    channel->id());
  query.bindValue(":type",       channel->type());
  query.bindValue(":gender",     channel->gender().raw());
  query.bindValue(":name",       channel->name());
  query.bindValue(":data",       JSON::generate(channel->data()));
  query.bindValue(":id",         channel->key());
  query.exec();
}
