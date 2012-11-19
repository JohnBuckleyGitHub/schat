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

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QThreadPool>
#include <QTimer>

#include "CacheDB.h"
#include "ChatCore.h"
#include "feeds/FeedStorage.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "sglobal.h"

QString CacheDB::m_id;
CacheDB *CacheDB::m_self = 0;

/*!
 * Задача для обновления или добавления канала в базу данных.
 */
class AddChannelTask : public QRunnable
{
  int m_gender;       ///< Пол.
  int m_type;         ///< Тип канала.
  QByteArray m_id;    ///< Идентификатор канала.
  qint64 m_key;       ///< Ключ в таблице.
  QString m_name;     ///< Имя канала.
  QVariantMap m_data; ///< JSON данные.

public:
  AddChannelTask(ClientChannel channel)
  : QRunnable()
  , m_gender(channel->gender().raw())
  , m_type(channel->type())
  , m_id(channel->id())
  , m_key(channel->key())
  , m_name(channel->name())
  , m_data(channel->data())
  {
  }

  void run()
  {
    if (m_key <= 0)
      m_key = CacheDB::key(m_id);

    QSqlQuery query(QSqlDatabase::database(CacheDB::id()));

    if (m_key > 0) {
      query.prepare(LS("UPDATE channels SET gender = :gender, name = :name, data = :data WHERE id = :id;"));
      query.bindValue(LS(":id"), m_key);
    }
    else {
      query.prepare(LS("INSERT INTO channels (channel, type, gender, name, data) VALUES (:channel, :type, :gender, :name, :data);"));
      query.bindValue(LS(":channel"), m_id);
      query.bindValue(LS(":type"),    m_type);
    }

    query.bindValue(LS(":gender"), m_gender);
    query.bindValue(LS(":name"),   m_name);
    query.bindValue(LS(":data"),   JSON::generate(m_data));
    query.exec();
  }
};


class ChannelDataTask : public QRunnable
{
  qint64 m_key;       ///< Ключ в таблице.
  QVariantMap m_data; ///< JSON данные.

public:
  ChannelDataTask(Channel *channel)
  : QRunnable()
  , m_key(channel->key())
  , m_data(channel->data())
  {
  }

  void run()
  {
    QSqlQuery query(QSqlDatabase::database(CacheDB::id()));
    query.prepare(LS("UPDATE channels SET data = :data WHERE id = :id;"));
    query.bindValue(LS(":data"), JSON::generate(m_data));
    query.bindValue(LS(":id"),   m_key);
    query.exec();
  }
};


class AddFeedTask : public QRunnable
{
  QByteArray m_id;    ///< Идентификатор канала.
  qint64 m_date;      ///< Дата модификации фида.
  QString m_name;     ///< Имя фида.
  QVariantMap m_body; ///< Тело фида.

public:
  AddFeedTask(FeedPtr feed, const FeedHeader &head)
  : QRunnable()
  , m_id(head.channel()->id())
  , m_date(head.date())
  , m_name(head.name())
  , m_body(feed->save())
  {
    if (head.data().size() == 2)
      m_body.remove(LS("head"));
  }


  void run()
  {
    qint64 channel = CacheDB::key(m_id);
    if (channel <= 0)
      return;

    QSqlQuery query(QSqlDatabase::database(CacheDB::id()));
    query.prepare(LS("SELECT id FROM feeds WHERE channel = :channel AND name = :name LIMIT 1;"));
    query.bindValue(LS(":channel"), channel);
    query.bindValue(LS(":name"), m_name);
    query.exec();

    qint64 key = -1;
    if (query.first())
      key = query.value(0).toLongLong();

    if (key == -1) {
      query.prepare(LS("INSERT INTO feeds (channel, date, name, json) VALUES (:channel, :date, :name, :json);"));
      query.bindValue(LS(":channel"), channel);
      query.bindValue(LS(":name"),    m_name);
    }
    else {
      query.prepare(LS("UPDATE feeds SET date = :date, json = :json WHERE id = :id;"));
      query.bindValue(LS(":id"), key);
    }

    query.bindValue(LS(":date"), m_date);
    query.bindValue(LS(":json"), JSON::generate(m_body));
    query.exec();
  }
};


CacheDB::CacheDB(QObject *parent)
  : QObject(parent)
{
  m_self = this;
}


/*!
 * Открытие базы данных.
 */
bool CacheDB::open(const QByteArray &id, const QString &dir)
{
  if (SimpleID::typeOf(id) != SimpleID::ServerId || dir.isEmpty())
    return false;

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
  qint64 key = CacheDB::key(id);
  if (key == -1)
    return ClientChannel();

  return channel(key, feeds);
}


/*!
 * Получение первичного ключа в таблице channels для канала с идентификатором \p id.
 *
 * \return первичный ключ или -1 в случае если канал не найден в кеше.
 */
qint64 CacheDB::key(const QByteArray &id)
{
  m_self->m_mutex.lock();
  qint64 key = m_self->m_cache.value(id);
  m_self->m_mutex.unlock();

  if (key)
    return key;

  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(LS("SELECT id FROM channels WHERE channel = :channel LIMIT 1;"));
  query.bindValue(LS(":channel"), id);
  query.exec();

  if (!query.first())
    return -1;

  key = query.value(0).toLongLong();
  setKey(id, key);
  return key;
}


/*!
 * Добавление или обновление канала.
 *
 * \return Ключ в таблице \b channels.
 */
void CacheDB::add(ClientChannel channel)
{
  AddChannelTask *task = new AddChannelTask(channel);
  m_self->m_tasks.append(task);
  if (m_self->m_tasks.size() == 1)
    QTimer::singleShot(0, m_self, SLOT(start()));
}


void CacheDB::add(FeedPtr feed)
{
  AddFeedTask *task = new AddFeedTask(feed, feed->head());
  m_self->m_tasks.append(task);
  if (m_self->m_tasks.size() == 1)
    QTimer::singleShot(0, m_self, SLOT(start()));
}


void CacheDB::clear()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec(LS("DROP TABLE IF EXISTS channels;"));
  query.exec(LS("DROP TABLE IF EXISTS feeds"));
  query.exec(LS("VACUUM;"));

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


void CacheDB::setData(Channel *channel)
{
  if (channel->key() <= 0)
    return;

  ChannelDataTask *task = new ChannelDataTask(channel);
  m_self->m_tasks.append(task);
  if (m_self->m_tasks.size() == 1)
    QTimer::singleShot(0, m_self, SLOT(start()));
}


void CacheDB::setKey(const QByteArray &id, qint64 key)
{
  if (key <= 0)
    return;

  QMutexLocker locker(&m_self->m_mutex);
  m_self->m_cache[id] = key;
}


void CacheDB::start()
{
  QThreadPool *pool = ChatCore::pool();
  while (!m_tasks.isEmpty())
    pool->start(m_tasks.takeFirst());
}


ClientChannel CacheDB::channel(qint64 id, bool feeds)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(LS("SELECT channel, gender, name, data FROM channels WHERE id = :id LIMIT 1;"));
  query.bindValue(LS(":id"), id);
  query.exec();

  if (!query.first())
    return ClientChannel();

  ClientChannel channel(new Channel(query.value(0).toByteArray(), query.value(2).toString()));
  setKey(channel->id(), id);
  channel->gender().setRaw(query.value(1).toLongLong());
  channel->setData(JSON::parse(query.value(3).toByteArray()).toMap());

  if (channel->type() == SimpleID::UserId)
    channel->setAccount();

  if (feeds)
    FeedStorage::load(channel.data());

  return channel;
}


/*!
 * Обновление схемы базы данных до версии 2.
 */
qint64 CacheDB::V2()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec(LS("DELETE FROM feeds;"));
  query.exec(LS("PRAGMA user_version = 2"));

  return 2;
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

  version();
}


/*!
 * Добавление в базу информации о версии, в будущем эта информация может быть использована для автоматического обновления схемы базы данных.
 */
void CacheDB::version()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec(LS("PRAGMA user_version"));
  if (!query.first())
    return;

  qint64 version = query.value(0).toLongLong();
  if (!version) {
    query.exec(LS("PRAGMA user_version = 2"));
    version = 2;
  }

  query.finish();

  if (version == 1) version = V2();
}
