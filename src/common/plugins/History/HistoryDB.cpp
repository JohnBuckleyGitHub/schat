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
#include "HistoryDB.h"
#include "net/SimpleID.h"
#include "sglobal.h"
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
    "  plain      TEXT"
    ");"));

  return true;
}


void HistoryDB::add(MessagePacket packet)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(LS("INSERT INTO messages (messageId, senderId, destId, status, date, command, text, plain) "
                     "VALUES (:messageId, :senderId, :destId, :status, :date, :command, :text, :plain);"));

  query.bindValue(LS(":messageId"), packet->id());
  query.bindValue(LS(":senderId"),  packet->sender());
  query.bindValue(LS(":destId"),    packet->dest());
  query.bindValue(LS(":status"),    packet->status());
  query.bindValue(LS(":date"),      packet->date());
  query.bindValue(LS(":command"),   packet->command());
  query.bindValue(LS(":text"),      packet->text());
  query.bindValue(LS(":plain"),     PlainTextFilter::filter(packet->text()));
  query.exec();
}


/*!
 * Закрытие базы данных и очистка кэша.
 */
void HistoryDB::close()
{
  QSqlDatabase::removeDatabase(m_id);
  m_id.clear();
}
