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

#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>

#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "HistoryDB.h"
#include "net/packets/message.h"
#include "User.h"

HistoryDB::HistoryDB(QObject *parent)
  : QObject(parent)
{
}


/*!
 * Добавление сообщения в историю.
 */
qint64 HistoryDB::add(int status, const MessageData &data, const QString &nick, const QString &plainText)
{
  qint64 result = addUser(data.senderId);
  if (result == -1)
    return -1;

  QSqlQuery query(QSqlDatabase::database(m_id));

  query.prepare(QLatin1String("INSERT INTO messages (senderId, destId, status, timestamp, name, nick, command, text, plainText) "
                     "VALUES (:senderId, :destId, :status, :timestamp, :name, :nick, :command, :text, :plainText);"));

  query.bindValue(QLatin1String(":senderId"), result);
  query.bindValue(QLatin1String(":destId"), data.destId);
  query.bindValue(QLatin1String(":status"), status);
  query.bindValue(QLatin1String(":timestamp"), data.timestamp);
  query.bindValue(QLatin1String(":name"), data.name);
  query.bindValue(QLatin1String(":nick"), nick);
  query.bindValue(QLatin1String(":command"), data.command);
  query.bindValue(QLatin1String(":text"), data.text);
  query.bindValue(QLatin1String(":plainText"), plainText);
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  query.exec(QLatin1String("SELECT last_insert_rowid();"));
  if (!query.first())
    return -1;

  return query.value(0).toLongLong();
}


qint64 HistoryDB::addUser(const QByteArray &id)
{
  ClientUser user = ChatCore::i()->client()->user(id);
  if (!user)
    return -1;

  if (m_cache.contains(id))
    return m_cache.value(id);

  qint64 result = updateUser(id);
  if (result != -1)
    return result;

  QSqlQuery query(QSqlDatabase::database(m_id));

  query.prepare(QLatin1String("INSERT INTO users (userId, nick, gender, host, status, userAgent) "
                     "VALUES (:userId, :nick, :gender, :host, :status, :userAgent);"));

  query.bindValue(QLatin1String(":userId"), id);
  query.bindValue(QLatin1String(":nick"), user->nick());
  query.bindValue(QLatin1String(":gender"), user->rawGender());
  query.bindValue(QLatin1String(":host"), user->host());
  query.bindValue(QLatin1String(":status"), user->statusToString());
  query.bindValue(QLatin1String(":userAgent"), user->userAgent());
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  query.exec(QLatin1String("SELECT last_insert_rowid();"));
  if (!query.first())
    return -1;

  result = query.value(0).toLongLong();
//  m_cache[id] = result;
  return result;
}


qint64 HistoryDB::isUser(const QByteArray &id) const
{
  return 0;
}


/*!
 * Обновление информации о пользователе.
 *
 * \param id Идентификатор пользователя.
 */
qint64 HistoryDB::updateUser(const QByteArray &id)
{
  ClientUser user = ChatCore::i()->client()->user(id);
  if (!user)
    return -1;

  qint64 index = -1;
  QSqlQuery query(QSqlDatabase::database(m_id));

  if (!m_cache.contains(id)) {
    query.prepare(QLatin1String("SELECT id FROM users WHERE userId = ? LIMIT 1;"));
    query.addBindValue(id);
    query.exec();

    if (query.first())
      index = query.value(0).toLongLong();
  }
  else
    index = m_cache.value(id);

  if (index == -1)
    return -1;

  query.prepare(QLatin1String("UPDATE users SET nick = :nick, gender = :gender, host = :host, status = :status, userAgent = :userAgent WHERE id = :id;"));
  query.bindValue(QLatin1String(":nick"), user->nick());
  query.bindValue(QLatin1String(":gender"), user->rawGender());
  query.bindValue(QLatin1String(":host"), user->host());
  query.bindValue(QLatin1String(":status"), user->statusToString());
  query.bindValue(QLatin1String(":userAgent"), user->userAgent());
  query.bindValue(QLatin1String(":id"), index);
  query.exec();

  return index;
}


/*!
 * Открытие базы данных.
 */
void HistoryDB::open(const QByteArray &id, const QString &dir)
{
  if (!m_id.isEmpty())
    close();

  m_id = id + QLatin1String("-history");

  QSqlDatabase db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), m_id);
  db.setDatabaseName(dir + QLatin1String("/history.sqlite"));
  if (!db.open())
    return;

  QSqlQuery query(db);
  query.exec(QLatin1String("PRAGMA synchronous = OFF"));

  query.exec(QLatin1String(
    "CREATE TABLE IF NOT EXISTS users ( "
    "  id         INTEGER PRIMARY KEY,"
    "  userId     BLOB    NOT NULL UNIQUE,"
    "  nick       TEXT,"
    "  gender     INTEGER,"
    "  host       TEXT,"
    "  status     TEXT,"
    "  userAgent  TEXT"
    ");"));

  query.exec(QLatin1String(
    "CREATE TABLE IF NOT EXISTS channels ( "
    "  id         INTEGER PRIMARY KEY,"
    "  channelId  BLOB    NOT NULL UNIQUE,"
    "  name       TEXT"
    ");"));

  query.exec(QLatin1String(
    "CREATE TABLE IF NOT EXISTS messages ( "
    "  id         INTEGER PRIMARY KEY,"
    "  senderId   INTEGER,"
    "  destId     BLOB,"
    "  status     INTEGER,"
    "  timestamp  INTEGER,"
    "  name       INTEGER,"
    "  nick       TEXT,"
    "  command    TEXT,"
    "  text       TEXT,"
    "  plainText  TEXT"
    ");"));
}


void HistoryDB::close()
{
  QSqlDatabase::removeDatabase(m_id);
}
