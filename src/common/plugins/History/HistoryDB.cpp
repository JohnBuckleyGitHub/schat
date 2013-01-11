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

#include <QRunnable>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QThreadPool>
#include <QTimer>
#include <QVariant>

#include "Channel.h"
#include "ChatCore.h"
#include "client/ChatClient.h"
#include "HistoryDB.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "text/PlainTextFilter.h"

QString HistoryDB::m_id;
HistoryDB *HistoryDB::m_self = 0;

class AddMessageTask : public QRunnable
{
  MessageNotice m_packet;

public:
  AddMessageTask(MessagePacket packet)
  : QRunnable()
  , m_packet(*packet)
  {}

  void run()
  {
    const QByteArray id = SimpleID::encode(m_packet.id());

    QSqlQuery query(QSqlDatabase::database(HistoryDB::id()));
    query.prepare(LS("SELECT id FROM messages WHERE messageId = :messageId LIMIT 1;"));
    query.bindValue(LS(":messageId"), id);
    query.exec();

    if (query.first() && query.value(0).toLongLong() > 0)
      return;

    query.prepare(LS("INSERT INTO messages (messageId, senderId, destId, status, date, command, text, plain, data) "
                       "VALUES (:messageId, :senderId, :destId, :status, :date, :command, :text, :plain, :data);"));

    query.bindValue(LS(":messageId"), id);
    query.bindValue(LS(":senderId"),  SimpleID::encode(m_packet.sender()));
    query.bindValue(LS(":destId"),    SimpleID::encode(m_packet.dest()));
    query.bindValue(LS(":status"),    HistoryDB::status(m_packet.status()));
    query.bindValue(LS(":date"),      m_packet.date());
    query.bindValue(LS(":command"),   m_packet.command());
    query.bindValue(LS(":text"),      m_packet.text());
    query.bindValue(LS(":plain"),     PlainTextFilter::filter(m_packet.text()));
    query.bindValue(LS(":data"),      m_packet.raw());
    query.exec();
  }
};


HistoryDB::HistoryDB(QObject *parent)
  : QObject(parent)
{
  m_self = this;
}


/*!
 * Открытие базы данных.
 *
 * Причина использования "PRAGMA synchronous = OFF": https://plus.google.com/109693334715857802924/posts/jEmzo7JRkWy
 * Если база уже была открыта, предыдущее соединение закрывается.
 * В случае необходимости создаются таблица messages:
 *
 * \param id  Идентификатор сервера, используется для формирования имени соединения с базой.
 * \param dir Папка, в которой находится база.
 */
bool HistoryDB::open(const QByteArray &id, const QString &dir)
{
  if (SimpleID::typeOf(id) != SimpleID::ServerId || dir.isEmpty())
    return false;

  QString newId = SimpleID::encode(id) + LS("-history");

  if (!m_id.isEmpty() && m_id == newId)
      return false;

  close();
  m_id = newId;

  QSqlDatabase db = QSqlDatabase::addDatabase(LS("QSQLITE"), m_id);
  db.setDatabaseName(dir + LS("/history.sqlite"));
  if (!db.open())
    return false;

  create();
  return true;
}


int HistoryDB::status(int status)
{
  if (status == Notice::OK)
    return Notice::Found;

  if (status == Notice::ChannelOffline || status == Notice::Unread)
    return Notice::Read;

  return status;
}


/*!
 * Получение последних сообщений.
 *
 * \param channel Кодированный идентификатор канала.
 * \param limit   Ограничение на количество сообщений.
 */
QList<QByteArray> HistoryDB::last(const QByteArray &channel, int limit)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  int type            = SimpleID::typeOf(channel);
  const QByteArray id = SimpleID::encode(ChatClient::id());

  if (type == SimpleID::ChannelId) {
    query.prepare(LS("SELECT messageId FROM messages WHERE destId = :destId ORDER BY id DESC LIMIT :limit;"));
    query.bindValue(LS(":destId"), channel);
  }
  else if (type == SimpleID::UserId) {
    if (id == channel) {
      query.prepare(LS("SELECT messageId FROM messages WHERE senderId = :senderId AND destId = :destId ORDER BY id DESC LIMIT :limit;"));
      query.bindValue(LS(":destId"), channel);
      query.bindValue(LS(":senderId"), channel);
    }
    else {
      query.prepare(LS("SELECT messageId FROM messages WHERE (senderId = :id1 AND destId = :id2) OR (senderId = :id3 AND destId = :id4) ORDER BY id DESC LIMIT :limit;"));
      query.bindValue(LS(":id1"), channel);
      query.bindValue(LS(":id2"), id);
      query.bindValue(LS(":id3"), id);
      query.bindValue(LS(":id4"), channel);
    }
  }
  else
    return QList<QByteArray>();

  query.bindValue(LS(":limit"), limit);
  query.exec();

  if (!query.isActive())
    return QList<QByteArray>();

  QList<QByteArray> out;
  while (query.next())
    out.prepend(query.value(0).toByteArray());

  return out;
}


MessageRecord HistoryDB::get(const QByteArray &id)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(LS("SELECT id, senderId, destId, status, date, command, text, data FROM messages WHERE messageId = :messageId LIMIT 1;"));

  query.bindValue(LS(":messageId"), SimpleID::encode(id));
  query.exec();

  if (!query.first())
    return MessageRecord();

  MessageRecord record;
  record.id        = query.value(0).toLongLong();
  record.messageId = id;
  record.senderId  = SimpleID::decode(query.value(1).toByteArray());
  record.destId    = SimpleID::decode(query.value(2).toByteArray());
  record.status    = query.value(3).toLongLong();
  record.date      = query.value(4).toLongLong();
  record.command   = query.value(5).toString();
  record.text      = query.value(6).toString();
  record.data      = query.value(7).toByteArray();

  return record;
}


void HistoryDB::add(MessagePacket packet)
{
  AddMessageTask *task = new AddMessageTask(packet);
  m_self->m_tasks.append(task);
  if (m_self->m_tasks.size() == 1)
    QTimer::singleShot(0, m_self, SLOT(startTasks()));
}


void HistoryDB::clear()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec("DROP TABLE IF EXISTS messages;");
  query.exec("VACUUM;");

  create();
}


/*!
 * Закрытие базы данных и очистка кэша.
 */
void HistoryDB::close()
{
  QSqlDatabase::removeDatabase(m_id);
  m_id.clear();
}


void HistoryDB::startTasks()
{
  QThreadPool *pool = ChatCore::pool();
  while (!m_tasks.isEmpty())
    pool->start(m_tasks.takeFirst());
}


void HistoryDB::create()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec(LS("PRAGMA synchronous = OFF"));

  query.exec(LS(
    "CREATE TABLE IF NOT EXISTS messages ( "
    "  id         INTEGER PRIMARY KEY,"
    "  messageId  BLOB,"
    "  senderId   BLOB,"
    "  destId     BLOB,"
    "  status     INTEGER DEFAULT ( 300 ),"
    "  date       INTEGER,"
    "  command    TEXT,"
    "  text       TEXT,"
    "  plain      TEXT,"
    "  data       BLOB"
    ");"));

  version();
}


/*!
 * Добавление в базу информации о версии, в будущем эта информация может быть использована для автоматического обновления схемы базы данных.
 */
void HistoryDB::version()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec(LS("PRAGMA user_version"));
  if (!query.first())
    return;

  qint64 version = query.value(0).toLongLong();
  if (!version) {
    query.exec(LS("PRAGMA user_version = 3"));
    version = 3;
    return;
  }

  query.finish();

  if (version == 1) version = V2();
  if (version == 2) version = V3();
}


qint64 HistoryDB::V2()
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.exec(LS("ALTER TABLE messages ADD data BLOB"));
  query.exec(LS("PRAGMA user_version = 2"));

  return 2;
}


qint64 HistoryDB::V3()
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
