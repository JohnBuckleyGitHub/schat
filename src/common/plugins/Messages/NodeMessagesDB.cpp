/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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
#include <QSqlError>
#include <QTimer>

#include "cores/Core.h"
#include "DataBase.h"
#include "net/packets/MessageNotice.h"
#include "NodeLog.h"
#include "NodeMessagesDB.h"
#include "sglobal.h"
#include "Storage.h"
#include "text/PlainTextFilter.h"

NodeMessagesDB *NodeMessagesDB::m_self = 0;
QString NodeMessagesDB::m_id;

NodeMessagesDB::NodeMessagesDB(QObject *parent)
  : QObject(parent)
{
  m_self = this;
  m_id = LS("messages");
}


NodeMessagesDB::~NodeMessagesDB()
{
  m_self = 0;
  m_id.clear();
}


bool NodeMessagesDB::open()
{
  QSqlDatabase db = QSqlDatabase::addDatabase(LS("QSQLITE"), m_id);
  db.setDatabaseName(Storage::var() + LS("/messages.sqlite"));
  if (!db.open()) {
    SCHAT_LOG_FATAL("Could not open DataBase file" << db.databaseName() << ":" << db.lastError());
    return false;
  }

  QSqlQuery query(db);
  query.exec(LS("PRAGMA synchronous = OFF"));

  query.exec(LS(
    "CREATE TABLE IF NOT EXISTS messages ( "
    "  id         INTEGER PRIMARY KEY,"
    "  messageId  BLOB NOT NULL UNIQUE,"
    "  senderId   BLOB NOT NULL,"
    "  destId     BLOB NOT NULL,"
    "  status     INTEGER DEFAULT ( 200 ),"
    "  date       INTEGER,"
    "  command    TEXT,"
    "  text       TEXT,"
    "  data       BLOB,"
    "  blob       BLOB"
    ");"));

  version();
  return true;
}


int NodeMessagesDB::status(int status)
{
  if (status == Notice::OK)
    return Notice::Found;

  if (status == Notice::ChannelOffline)
    return Notice::Unread;

  return status;
}


/*!
 * Получения списка сообщений по их идентификаторам.
 */
QList<MessageRecord> NodeMessagesDB::get(const QList<QByteArray> &ids, const QByteArray &userId)
{
  if (ids.isEmpty())
    return QList<MessageRecord>();

  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(LS("SELECT id, senderId, destId, status, date, command, text, data FROM messages WHERE messageId = :messageId LIMIT 1;"));

  QList<MessageRecord> out;
# if QT_VERSION >= 0x040700
  out.reserve(ids.size());
# endif

  for (int i = 0; i < ids.size(); ++i) {
    const QByteArray &id = ids.at(i);
    query.bindValue(LS(":messageId"), SimpleID::encode(id));
    query.exec();

    if (!query.first())
      continue;

    MessageRecord record;
    record.id        = query.value(0).toLongLong();
    record.messageId = id;
    record.senderId  = SimpleID::decode(query.value(1).toByteArray());
    record.destId    = SimpleID::decode(query.value(2).toByteArray());
    if (!userId.isEmpty() && (record.senderId != userId && record.destId != userId))
      continue;

    record.status    = query.value(3).toLongLong();
    record.date      = query.value(4).toLongLong();
    record.command   = query.value(5).toString();
    record.text      = query.value(6).toString();
    record.data      = query.value(7).toByteArray();
    out.append(record);
  }

  return out;
}


QList<MessageRecord> NodeMessagesDB::messages(QSqlQuery &query)
{
  if (!query.isActive())
    return QList<MessageRecord>();

  QList<MessageRecord> out;

  while (query.next()) {
    MessageRecord record;
    record.id        = query.value(0).toLongLong();
    record.messageId = SimpleID::decode(query.value(1).toByteArray());
    record.senderId  = SimpleID::decode(query.value(2).toByteArray());
    record.destId    = SimpleID::decode(query.value(3).toByteArray());
    record.status    = query.value(4).toLongLong();
    record.date      = query.value(5).toLongLong();
    record.command   = query.value(6).toString();
    record.text      = query.value(7).toString();
    record.data      = query.value(8).toByteArray();
    out.prepend(record);
  }

  return out;
}


QList<MessageRecord> NodeMessagesDB::offline(const QByteArray &user)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(LS("SELECT id, messageId, senderId, destId, status, date, command, text, data FROM messages WHERE destId = :destId AND status = 301 ORDER BY id DESC;"));
  query.bindValue(LS(":destId"), SimpleID::encode(user));
  query.exec();

  return messages(query);
}


/*!
 * Получение не кодированных идентификаторов последних сообщений для обычного канала или собственного канала пользователя.
 *
 * \param channel Не кодированный идентификатор канала.
 * \param limit   Максимальное количество сообщений.
 * \param before  Дата, для загрузки сообщений старее этой даты.
 */
QList<QByteArray> NodeMessagesDB::last(const QByteArray &channel, int limit, qint64 before)
{
  if (SimpleID::typeOf(channel) != SimpleID::ChannelId)
    return QList<QByteArray>();

  QSqlQuery query(QSqlDatabase::database(m_id));

  if (before) {
    query.prepare(LS("SELECT messageId FROM messages WHERE destId = :destId AND date < :before ORDER BY id DESC LIMIT :limit;"));
    query.bindValue(LS(":before"), before);
  }
  else
    query.prepare(LS("SELECT messageId FROM messages WHERE destId = :destId ORDER BY id DESC LIMIT :limit;"));

  query.bindValue(LS(":destId"), SimpleID::encode(channel));
  query.bindValue(LS(":limit"), limit);
  query.exec();

  return ids(query);
}


/*!
 * Получение не кодированных идентификаторов последних приватных сообщений между двумя пользователями.
 *
 * \param user1  Не кодированный идентификатор одного пользователя.
 * \param user2  Не кодированный идентификатор другого пользователя.
 * \param limit  Максимальное количество сообщений.
 * \param before Дата, для загрузки сообщений старее этой даты.
 */
QList<QByteArray> NodeMessagesDB::last(const QByteArray &user1, const QByteArray &user2, int limit, qint64 before)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  if (before) {
    query.prepare(LS("SELECT messageId FROM messages WHERE ((senderId = :id1 AND destId = :id2) OR (senderId = :id3 AND destId = :id4)) AND date < :before ORDER BY id DESC LIMIT :limit;"));
    query.bindValue(LS(":before"), before);
  }
  else
    query.prepare(LS("SELECT messageId FROM messages WHERE (senderId = :id1 AND destId = :id2) OR (senderId = :id3 AND destId = :id4) ORDER BY id DESC LIMIT :limit;"));

  const QByteArray u1 = SimpleID::encode(user1);
  const QByteArray u2 = SimpleID::encode(user2);

  query.bindValue(LS(":id1"), u1);
  query.bindValue(LS(":id2"), u2);
  query.bindValue(LS(":id3"), u2);
  query.bindValue(LS(":id4"), u1);
  query.bindValue(LS(":limit"), limit);
  query.exec();

  return ids(query);
}


QList<QByteArray> NodeMessagesDB::since(const QByteArray &channel, qint64 start, qint64 end)
{
  if (SimpleID::typeOf(channel) != SimpleID::ChannelId)
    return QList<QByteArray>();

  QSqlQuery query(QSqlDatabase::database(m_id));

  query.prepare(LS("SELECT messageId FROM messages WHERE date > :start AND date < :end AND destId = :destId ORDER BY id DESC;"));
  query.bindValue(LS(":destId"), SimpleID::encode(channel));
  query.bindValue(LS(":start"),  start);
  query.bindValue(LS(":end"),    end);
  query.exec();

  return ids(query);
}


QList<QByteArray> NodeMessagesDB::since(const QByteArray &user1, const QByteArray &user2, qint64 start, qint64 end)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(LS("SELECT messageId FROM messages WHERE date > :start AND date < :end AND ((senderId = :id1 AND destId = :id2) OR (senderId = :id3 AND destId = :id4)) ORDER BY id DESC;"));

  const QByteArray u1 = SimpleID::encode(user1);
  const QByteArray u2 = SimpleID::encode(user2);

  query.bindValue(LS(":id1"),   u1);
  query.bindValue(LS(":id2"),   u2);
  query.bindValue(LS(":id3"),   u2);
  query.bindValue(LS(":id4"),   u1);
  query.bindValue(LS(":start"), start);
  query.bindValue(LS(":end"),   end);
  query.exec();

  return ids(query);
}


void NodeMessagesDB::add(const MessageNotice &packet, int status)
{
  AddMessageTask *task = new AddMessageTask(packet, status);
  m_self->m_tasks.append(task);
  if (m_self->m_tasks.size() == 1)
    QTimer::singleShot(0, m_self, SLOT(startTasks()));
}


void NodeMessagesDB::markAsRead(const QList<MessageRecord> &records)
{
  if (records.isEmpty())
    return;

  QSqlDatabase db = QSqlDatabase::database(m_id);
  QSqlQuery query(db);
  db.transaction();
  query.prepare(LS("UPDATE messages SET status = 302 WHERE id = :id;"));

  for (int i = 0; i < records.size(); ++i) {
    const MessageRecord& record = records.at(i);
    if (!record.id)
      continue;

    query.bindValue(LS(":id"), record.id);
    query.exec();
  }

  db.commit();
}


void NodeMessagesDB::startTasks()
{
  if (m_tasks.isEmpty())
    return;

  QThreadPool *pool = DataBase::pool();
  while (!m_tasks.isEmpty())
    pool->start(m_tasks.takeFirst());
}


/*!
 * Добавление в базу информации о версии, в будущем эта информация может быть использована для автоматического обновления схемы базы данных.
 */
void NodeMessagesDB::version()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec(LS("PRAGMA user_version"));
  if (!query.first())
    return;

  qint64 version = query.value(0).toLongLong();
  if (!version) {
    query.exec(LS("PRAGMA user_version = 4"));
    version = 4;
    return;
  }

  query.finish();

  if (version == 1) version = V2();
  if (version == 2) version = V3();
  if (version == 3) version = V4();
}


QList<QByteArray> NodeMessagesDB::ids(QSqlQuery &query)
{
  if (!query.isActive())
    return QList<QByteArray>();

  QList<QByteArray> out;
  while (query.next())
    out.prepend(SimpleID::decode(query.value(0).toByteArray()));

  return out;
}


/*!
 * Обновление схемы базы до версии 2.
 */
qint64 NodeMessagesDB::V2()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec(LS("ALTER TABLE messages ADD data BLOB"));
  query.exec(LS("PRAGMA user_version = 2"));

  return 2;
}


qint64 NodeMessagesDB::V3()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec(LS("BEGIN TRANSACTION;"));

  query.prepare(LS("SELECT id, messageId, senderId, destId FROM messages"));
  query.exec();

  QSqlQuery update(QSqlDatabase::database(m_id));
  update.prepare(LS("UPDATE messages SET messageId = :messageId, senderId = :senderId, destId = :destId WHERE id = :id;"));

  while (query.next()) {
    update.bindValue(LS(":id"),        query.value(0));
    update.bindValue(LS(":messageId"), SimpleID::encode(query.value(1).toByteArray()));
    update.bindValue(LS(":senderId"),  SimpleID::encode(query.value(2).toByteArray()));
    update.bindValue(LS(":destId"),    SimpleID::encode(query.value(3).toByteArray()));
    update.exec();
  }

  query.exec(LS("PRAGMA user_version = 3"));
  query.exec(LS("COMMIT;"));

  return 3;
}


qint64 NodeMessagesDB::V4()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec(LS("BEGIN TRANSACTION;"));
  query.exec(LS("ALTER TABLE messages RENAME TO messages_tmp;"));
  query.exec(LS(
    "CREATE TABLE messages ( "
    "  id         INTEGER PRIMARY KEY,"
    "  messageId  BLOB NOT NULL UNIQUE,"
    "  senderId   BLOB NOT NULL,"
    "  destId     BLOB NOT NULL,"
    "  status     INTEGER DEFAULT ( 200 ),"
    "  date       INTEGER,"
    "  command    TEXT,"
    "  text       TEXT,"
    "  data       BLOB,"
    "  blob       BLOB"
    ");"
  ));

  query.exec(LS("INSERT INTO messages (messageId, senderId, destId, status, date, command, text, data) SELECT messageId, senderId, destId, status, date, command, text, data FROM messages_tmp;"));
  query.exec(LS("DROP TABLE messages_tmp;"));
  query.exec(LS("PRAGMA user_version = 4"));
  query.exec(LS("COMMIT;"));

  query.exec(LS("VACUUM;"));

  return 4;
}


AddMessageTask::AddMessageTask(const MessageNotice &packet, int status)
  : QRunnable()
  , m_status(status)
  , m_packet(packet)
{
}


void AddMessageTask::run()
{
  QSqlQuery query(QSqlDatabase::database(NodeMessagesDB::id()));
  query.prepare(LS("INSERT INTO messages (messageId,  senderId,  destId,  status,  date,  command,  text,  data) "
                                 "VALUES (:messageId, :senderId, :destId, :status, :date, :command, :text, :data);"));

  query.bindValue(LS(":messageId"), SimpleID::encode(m_packet.id()));
  query.bindValue(LS(":senderId"),  SimpleID::encode(m_packet.sender()));
  query.bindValue(LS(":destId"),    SimpleID::encode(m_packet.dest()));
  query.bindValue(LS(":status"),    NodeMessagesDB::status(m_status));
  query.bindValue(LS(":date"),      m_packet.date());
  query.bindValue(LS(":command"),   m_packet.command());
  query.bindValue(LS(":text"),      m_packet.text());
  query.bindValue(LS(":data"),      m_packet.raw());
  query.exec();
}
