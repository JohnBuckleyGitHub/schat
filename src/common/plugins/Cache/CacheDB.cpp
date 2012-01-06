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

#include "CacheDB.h"
#include "net/SimpleID.h"

QString CacheDB::m_id;

/*!
 * Закрытие базы данных.
 */
void CacheDB::close()
{
  m_id.clear();
  QSqlDatabase::removeDatabase(m_id);
}


/*!
 * Открытие базы данных.
 */
void CacheDB::open(const QByteArray &id, const QString &dir)
{
  if (!m_id.isEmpty())
    close();

  m_id = SimpleID::encode(id) + "-cache";

  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_id);
  db.setDatabaseName(dir + "/cache.sqlite");
  if (!db.open())
    return;

  QSqlQuery query(db);
  query.exec("PRAGMA synchronous = OFF");

  query.exec(
    "CREATE TABLE IF NOT EXISTS channels ( "
    "  id         INTEGER PRIMARY KEY,"
    "  channel    BLOB    NOT NULL UNIQUE,"
    "  type       INTEGER DEFAULT ( 73 ),"
    "  gender     INTEGER DEFAULT ( 0 ),"
    "  name       TEXT    NOT NULL,"
    "  data       BLOB"
    ");"
  );
}
