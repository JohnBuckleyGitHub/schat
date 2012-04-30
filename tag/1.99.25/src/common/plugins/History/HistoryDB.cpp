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
#include <QStringList>
#include <QVariant>

#include "Channel.h"
#include "client/ChatClient.h"
#include "HistoryDB.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "text/MessageId.h"
#include "text/PlainTextFilter.h"

QString HistoryDB::m_id;

HistoryDB::HistoryDB(QObject *parent)
  : QObject(parent)
{
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


QList<MessageId> HistoryDB::last(const QByteArray &channel, int limit)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  int type = SimpleID::typeOf(channel);

  if (type == SimpleID::ChannelId) {
    query.prepare(LS("SELECT id, messageId, date FROM messages WHERE destId = :destId ORDER BY id DESC LIMIT :limit;"));
    query.bindValue(LS(":destId"), channel);
  }
  else if (type == SimpleID::UserId) {
    if (ChatClient::id() == channel) {
      query.prepare(LS("SELECT id, messageId, date FROM messages WHERE senderId = :senderId AND destId = :destId ORDER BY id DESC LIMIT :limit;"));
      query.bindValue(LS(":destId"), channel);
      query.bindValue(LS(":senderId"), channel);
    }
    else {
      query.prepare(LS("SELECT id, messageId, date FROM messages WHERE (senderId = :id1 AND destId = :id2) OR (senderId = :id3 AND destId = :id4) ORDER BY id DESC LIMIT :limit;"));
      query.bindValue(LS(":id1"), channel);
      query.bindValue(LS(":id2"), ChatClient::id());
      query.bindValue(LS(":id3"), ChatClient::id());
      query.bindValue(LS(":id4"), channel);
    }
  }
  else
    return QList<MessageId>();

  query.bindValue(LS(":limit"), limit);
  query.exec();

  if (!query.isActive())
    return QList<MessageId>();

  QList<MessageId> out;
  while (query.next())
    out.prepend(MessageId(query.value(2).toLongLong(), query.value(1).toByteArray()));

  return out;
}


QVariantList HistoryDB::get(const MessageId &id)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(LS("SELECT id, senderId, destId, status, command, text FROM messages WHERE messageId = :messageId AND date = :date LIMIT 1;"));

  query.bindValue(LS(":messageId"), id.id());
  query.bindValue(LS(":date"), id.date());
  query.exec();

  QVariantList out;
  if (!query.first())
    return out;

  out.append(id.id());          // 0 messageId
  out.append(query.value(1));   // 1 senderId
  out.append(query.value(2));   // 2 destId
  out.append(query.value(3));   // 3 status
  out.append(id.date());        // 4 date
  out.append(query.value(4));   // 5 command
  out.append(query.value(5));   // 6 text
  out.append(query.value(0));   // 7 id

  return out;
}


void HistoryDB::add(MessagePacket packet)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(LS("SELECT id FROM messages WHERE messageId = :messageId AND date = :date LIMIT 1;"));
  query.bindValue(LS(":messageId"), packet->id());
  query.bindValue(LS(":date"), packet->date());
  query.exec();

  if (query.first() && query.value(0).toLongLong() > 0)
    return;

  query.prepare(LS("INSERT INTO messages (messageId, senderId, destId, status, date, command, text, plain) "
                     "VALUES (:messageId, :senderId, :destId, :status, :date, :command, :text, :plain);"));

  query.bindValue(LS(":messageId"), packet->id());
  query.bindValue(LS(":senderId"),  packet->sender());
  query.bindValue(LS(":destId"),    packet->dest());
  query.bindValue(LS(":status"),    status(packet->status()));
  query.bindValue(LS(":date"),      packet->date());
  query.bindValue(LS(":command"),   packet->command());
  query.bindValue(LS(":text"),      packet->text());
  query.bindValue(LS(":plain"),     PlainTextFilter::filter(packet->text()));
  query.exec();
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
    "  plain      TEXT"
    ");"));
}
