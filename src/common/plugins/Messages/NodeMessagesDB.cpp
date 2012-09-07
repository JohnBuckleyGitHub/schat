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
#include <QSqlError>
#include <QTimer>

#include "cores/Core.h"
#include "net/packets/MessageNotice.h"
#include "NodeMessagesDB.h"
#include "sglobal.h"
#include "Storage.h"
#include "text/PlainTextFilter.h"
#include "DataBase.h"

bool NodeMessagesDB::m_isOpen = true;
NodeMessagesDB *NodeMessagesDB::m_self = 0;
QString NodeMessagesDB::m_id;

NodeMessagesDB::NodeMessagesDB(QObject *parent)
  : QObject(parent)
{
  m_self = this;
  m_id = LS("messages");
}


bool NodeMessagesDB::open()
{
  QSqlDatabase db = QSqlDatabase::addDatabase(LS("QSQLITE"), m_id);
  db.setDatabaseName(Storage::varPath() + LS("/messages.sqlite"));
  if (!db.open())
    return false;

  QSqlQuery query(db);
  query.exec(LS("PRAGMA synchronous = OFF"));

  query.exec(LS(
    "CREATE TABLE IF NOT EXISTS messages ( "
    "  id         INTEGER PRIMARY KEY,"
    "  messageId  BLOB,"
    "  senderId   BLOB,"
    "  destId     BLOB,"
    "  status     INTEGER DEFAULT ( 200 ),"
    "  date       INTEGER,"
    "  command    TEXT,"
    "  text       TEXT,"
    "  plain      TEXT,"
    "  data       BLOB"
    ");"));

  version();
  m_isOpen = true;
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
 * Получение идентификаторов последних сообщений для обычного канала или собственного канала пользователя.
 *
 * \param channel Идентификатор канала.
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

  query.bindValue(LS(":destId"), channel);
  query.bindValue(LS(":limit"), limit);
  query.exec();

  return ids(query);
}


/*!
 * Получение идентификаторов последних приватных сообщений между двумя пользователями.
 *
 * \param user1  Идентификатор одного пользователя.
 * \param user2  Идентификатор другого пользователя.
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

  query.bindValue(LS(":id1"), user1);
  query.bindValue(LS(":id2"), user2);
  query.bindValue(LS(":id3"), user2);
  query.bindValue(LS(":id4"), user1);
  query.bindValue(LS(":limit"), limit);
  query.exec();

  return ids(query);
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
    query.bindValue(LS(":messageId"), id);
    query.exec();

    if (!query.first())
      continue;

    MessageRecord record;
    record.id        = query.value(0).toLongLong();
    record.messageId = id;
    record.senderId  = query.value(1).toByteArray();
    record.destId    = query.value(2).toByteArray();
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
    record.messageId = query.value(1).toByteArray();
    record.senderId  = query.value(2).toByteArray();
    record.destId    = query.value(3).toByteArray();
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
  query.bindValue(LS(":destId"), user);
  query.exec();

  return messages(query);
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
    query.exec(LS("PRAGMA user_version = 2"));
    version = 2;
    return;
  }

  if (version == 1)
    V2();
}


QList<QByteArray> NodeMessagesDB::ids(QSqlQuery &query)
{
  if (!query.isActive())
    return QList<QByteArray>();

  QList<QByteArray> out;
  while (query.next())
    out.prepend(query.value(0).toByteArray());

  return out;
}


/*!
 * Обновление схемы базы до версии 2.
 */
void NodeMessagesDB::V2()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec(LS("ALTER TABLE messages ADD data BLOB"));
  query.exec(LS("PRAGMA user_version = 2"));
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
  query.prepare(LS("INSERT INTO messages (messageId,  senderId,  destId,  status,  date,  command,  text,  plain,  data) "
                                 "VALUES (:messageId, :senderId, :destId, :status, :date, :command, :text, :plain, :data);"));

  query.bindValue(LS(":messageId"), m_packet.id());
  query.bindValue(LS(":senderId"),  m_packet.sender());
  query.bindValue(LS(":destId"),    m_packet.dest());
  query.bindValue(LS(":status"),    NodeMessagesDB::status(m_status));
  query.bindValue(LS(":date"),      m_packet.date());
  query.bindValue(LS(":command"),   m_packet.command());
  query.bindValue(LS(":text"),      m_packet.text());
  query.bindValue(LS(":plain"),     PlainTextFilter::filter(m_packet.text()));
  query.bindValue(LS(":data"),      m_packet.raw());
  query.exec();
}
