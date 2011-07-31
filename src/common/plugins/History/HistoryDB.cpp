/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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
#include <QStringList>
#include <QSqlQuery>

#include "HistoryDB.h"

HistoryDB::HistoryDB(QObject *parent)
  : QObject(parent)
{
}


void HistoryDB::open(const QByteArray &id, const QString &dir)
{
  if (!m_id.isEmpty())
    close();

  m_id = id + QLatin1String("-history");

  QSqlDatabase db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), m_id);
  db.setDatabaseName(dir + QLatin1String("/history.sqlite"));
  if (!db.open())
    return;

  QStringList tables = db.tables();

  if (!tables.contains(QLatin1String("users"))) {
    QSqlQuery query(QLatin1String(
    "CREATE TABLE users ( "
    "  id         INTEGER PRIMARY KEY,"
    "  userId     BLOB    NOT NULL UNIQUE,"
    "  nick       TEXT,"
    "  gender     INTEGER DEFAULT ( 0 ),"
    "  host       TEXT,"
    "  status     TEXT,"
    "  userAgent  TEXT"
    ");"), db);
  }

  if (!tables.contains(QLatin1String("channels"))) {
    QSqlQuery query(QLatin1String(
    "CREATE TABLE channels ( "
    "  id         INTEGER PRIMARY KEY,"
    "  channelId  BLOB    NOT NULL UNIQUE,"
    "  name       TEXT"
    ");"), db);
  }

  if (!tables.contains(QLatin1String("messages"))) {
    QSqlQuery query(QLatin1String(
    "CREATE TABLE messages ( "
    "  id         INTEGER PRIMARY KEY,"
    "  senderId   BLOB    NOT NULL UNIQUE,"
    "  destId     BLOB    NOT NULL UNIQUE,"
    "  timestamp  INTEGER DEFAULT ( 0 ),"
    "  name       INTEGER DEFAULT ( 0 ),"
    "  text       TEXT"
    ");"), db);
  }
}


void HistoryDB::close()
{
  QSqlDatabase::removeDatabase(m_id);
}
