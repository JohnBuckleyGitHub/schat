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

#include "cores/Core.h"
#include "FileLocations.h"
#include "net/packets/MessageNotice.h"
#include "NodeMessagesDB.h"
#include "sglobal.h"
#include "Storage.h"
#include "text/PlainTextFilter.h"

bool NodeMessagesDB::m_isOpen = true;
QString NodeMessagesDB::m_id = LS("messages");

NodeMessagesDB::NodeMessagesDB()
{
}


bool NodeMessagesDB::open()
{
  QSqlDatabase db = QSqlDatabase::addDatabase(LS("QSQLITE"), m_id);
  db.setDatabaseName(Storage::locations()->path(FileLocations::VarPath) + LS("/messages.sqlite"));
  if (!db.open())
    return false;

  QSqlQuery query(db);
  query.exec(LS("PRAGMA synchronous = OFF"));

  query.exec(QLatin1String(
    "CREATE TABLE IF NOT EXISTS messages ( "
    "  id         INTEGER PRIMARY KEY,"
    "  messageId  BLOB,"
    "  senderId   BLOB,"
    "  destId     BLOB,"
    "  status     INTEGER DEFAULT ( 200 ),"
    "  date       INTEGER,"
    "  text       TEXT,"
    "  plain      TEXT"
    ");"));

  m_isOpen = true;
  return true;
}


void NodeMessagesDB::add(const MessageNotice &packet, int status)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(LS("INSERT INTO messages (messageId, senderId, destId, status, date, text, plain) "
                     "VALUES (:messageId, :senderId, :destId, :status, :date, :text, :plain);"));

  query.bindValue(LS(":messageId"), packet.id());
  query.bindValue(LS(":senderId"),  packet.sender());
  query.bindValue(LS(":destId"),    packet.dest());
  query.bindValue(LS(":status"),    status);
  query.bindValue(LS(":date"),      Core::date());
  query.bindValue(LS(":text"),      packet.text());
  query.bindValue(LS(":plain"),     PlainTextFilter::filter(packet.text()));
  query.exec();
}