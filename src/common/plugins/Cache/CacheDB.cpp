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
#include "feeds/FeedStorage.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "sglobal.h"

QString CacheDB::m_id;
CacheDB *CacheDB::m_self = 0;

class AddChannelTask : public QRunnable
{
public:
  AddChannelTask(ClientChannel channel)
  : m_gender(channel->gender().raw())
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
      m_key = CacheDB::key(m_id, m_type);

    if (m_key > 0)
      update();
    else
      add();
  }

private:
  /*!
   * Добавление канала в базу данных.
   */
  void add()
  {
    QSqlQuery query(QSqlDatabase::database(CacheDB::id()));
    query.prepare(LS("INSERT INTO channels (channel, type, gender, name, data) "
                       "VALUES (:channel, :type, :gender, :name, :data);"));

    query.bindValue(LS(":channel"), m_id);
    query.bindValue(LS(":type"),    m_type);
    query.bindValue(LS(":gender"),  m_gender);
    query.bindValue(LS(":name"),    m_name);
    query.bindValue(LS(":data"),    JSON::generate(m_data));
    query.exec();
  }

  /*!
   * Обновление информации о канале.
   */
  void update()
  {
    QSqlQuery query(QSqlDatabase::database(CacheDB::id()));
    query.prepare(LS("UPDATE channels SET gender = :gender, name = :name, data = :data WHERE id = :id;"));
    query.bindValue(LS(":gender"), m_gender);
    query.bindValue(LS(":name"),   m_name);
    query.bindValue(LS(":data"),   JSON::generate(m_data));
    query.bindValue(LS(":id"),     m_key);
    query.exec();
  }

  int m_gender;       ///< Пол.
  int m_type;         ///< Тип канала.
  QByteArray m_id;    ///< Идентификатор канала.
  qint64 m_key;       ///< Ключ в таблице.
  QString m_name;     ///< Имя канала.
  QVariantMap m_data; ///< JSON данные.
};


CacheDB::CacheDB(QObject *parent)
  : QObject(parent)
{
  m_self = this;
  m_pool = new QThreadPool(this);
  m_pool->setMaxThreadCount(1);
}


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
  qint64 key = CacheDB::key(id, SimpleID::typeOf(id));
  if (key == -1)
    return ClientChannel();

  return channel(key, feeds);
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
  channel->setKey(id);
  channel->gender().setRaw(query.value(1).toLongLong());
  channel->setData(JSON::parse(query.value(3).toByteArray()).toMap());

  if (feeds)
    FeedStorage::load(channel.data());

  return channel;
}


/*!
 * Возвращает ключ в таблице \b channels.
 * Функция поддерживает кэширование ключа в канале.
 *
 * \param channel Канал.
 *
 * \return Ключ в таблице или -1 в случае ошибки.
 */
qint64 CacheDB::key(Channel *channel)
{
  if (channel->key() > 0)
    return channel->key();

  qint64 key = CacheDB::key(channel->id(), channel->type());
  if (key <= 0)
    return -1;

  channel->setKey(key);
  return key;
}


/*!
 * Возвращает ключ в таблице \b channels на основе идентификатора канала и типа канала.
 *
 * \param id   Идентификатор канала.
 * \param type Тип канала.
 *
 * \return Ключ в таблице или -1 в случае ошибки.
 */
qint64 CacheDB::key(const QByteArray &id, int type)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(LS("SELECT id FROM channels WHERE channel = :id AND type = :type LIMIT 1;"));
  query.bindValue(LS(":id"),   id);
  query.bindValue(LS(":type"), type);
  query.exec();

  if (!query.first())
    return -1;

  return query.value(0).toLongLong();
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


void CacheDB::start()
{
  while (!m_tasks.isEmpty())
    m_pool->start(m_tasks.takeFirst());
}
