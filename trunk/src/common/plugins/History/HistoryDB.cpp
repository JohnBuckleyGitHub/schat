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


/*!
 * Закрытие базы данных и очистка кэша.
 */
void HistoryDB::close()
{
  QSqlDatabase::removeDatabase(m_id);
  m_id.clear();
}
