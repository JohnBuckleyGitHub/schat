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
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>

#include "DataBase.h"
#include "FileLocations.h"
#include "net/SimpleID.h"
#include "Storage.h"

DataBase::DataBase(QObject *parent)
  : QObject(parent)
{
}


/*!
 * Поиск пользователя по уникальному идентификатору.
 *
 * \param id Идентификатор пользователя.
 * \return Указатель на пользователя или 0 если он не был найден.
 */
ChatUser DataBase::user(const QByteArray &id)
{
  QSqlQuery query;
  query.prepare(QLatin1String("SELECT id, userId, nick, normalNick, gender, ip, userAgent FROM users WHERE userId = ? LIMIT 1;"));
  query.addBindValue(id);
  query.exec();

  if (!query.first())
    return ChatUser();

  return ChatUser(new ServerUser(query));
}


/*!
 * Поиск пользователя по ключу в таблице users.
 *
 * \param key Ключ в таблице users.
 * \return Указатель на пользователя или 0 если он не был найден.
 */

ChatUser DataBase::user(qint64 key)
{
  QSqlQuery query;
  query.prepare(QLatin1String("SELECT id, userId, nick, normalNick, gender, ip, userAgent FROM users WHERE id = ? LIMIT 1;"));
  query.addBindValue(key);
  query.exec();

  if (!query.first())
    return ChatUser();

  return ChatUser(new ServerUser(query));
}


int DataBase::start()
{
  m_db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"));
  m_db.setDatabaseName(Storage::i()->locations()->path(FileLocations::VarPath) + QLatin1String("/") + Storage::i()->locations()->path(FileLocations::BaseName) + QLatin1String(".sqlite"));
  if (!m_db.open())
    return -1;

  QSqlQuery query;
  query.exec(QLatin1String("PRAGMA synchronous = OFF"));
  QStringList tables = m_db.tables();

  if (!tables.contains(QLatin1String("users"))) {
    query.exec(QLatin1String(
    "CREATE TABLE users ( "
    "  id         INTEGER PRIMARY KEY,"
    "  userId     BLOB    NOT NULL UNIQUE,"
    "  [group]    INTEGER DEFAULT ( 4 ),"
    "  nick       TEXT,"
    "  normalNick TEXT,"
    "  gender     INTEGER DEFAULT ( 0 ),"
    "  ip         TEXT,"
    "  userAgent  TEXT"
    ");"));
  }

  if (!tables.contains(QLatin1String("groups"))) {
    query.exec(QLatin1String(
    "CREATE TABLE groups ( "
    "  id    INTEGER PRIMARY KEY,"
    "  name  TEXT    UNIQUE,"
    "  allow INTEGER DEFAULT ( 0 ),"
    "  deny  INTEGER DEFAULT ( 0 )"
    ");"));

    addGroup("admin");
    addGroup("moderator");
    addGroup("user");
    addGroup("anonymous");
  }

  return 0;
}


/*!
 * Добавление нового пользователя в базу данных.
 *
 * \param user Указатель на пользователя.
 * \return Ключ в таблице users или -1 в случае ошибки.
 */
qint64 DataBase::add(ChatUser user)
{
  QSqlQuery query;
  query.prepare(QLatin1String("INSERT INTO users (userId, nick, normalNick, gender, ip, userAgent) "
                     "VALUES (?, ?, ?, ?, ?, ?);"));

  query.addBindValue(user->id());
  query.addBindValue(user->nick());
  query.addBindValue(user->normalNick());
  query.addBindValue(user->rawGender());
  query.addBindValue(user->host());
  query.addBindValue(user->userAgent());
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  query.exec(QLatin1String("SELECT last_insert_rowid();"));
  if (!query.first())
    return -1;

  qint64 key = query.value(0).toLongLong();
  user->setKey(key);

  return key;
}


qint64 DataBase::addGroup(const QString &name, qint64 allow, qint64 deny)
{
  QSqlQuery query;
  query.prepare(QLatin1String("INSERT INTO groups (name, allow, deny) VALUES (?, ?, ?);"));
  query.addBindValue(name);
  query.addBindValue(allow);
  query.addBindValue(deny);
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  query.exec(QLatin1String("SELECT last_insert_rowid();"));
  if (!query.first())
    return -1;

  return query.value(0).toLongLong();
}


/*!
 * Обновление информации о пользователе.
 */
void DataBase::update(ChatUser user)
{
  QSqlQuery query;
  query.prepare(QLatin1String("UPDATE users SET nick = ?, normalNick = ?, gender = ?, ip = ?, userAgent = ? WHERE id = ?;"));
  query.addBindValue(user->nick());
  query.addBindValue(user->normalNick());
  query.addBindValue(user->rawGender());
  query.addBindValue(user->host());
  query.addBindValue(user->userAgent());
  query.addBindValue(user->key());
  query.exec();
}
