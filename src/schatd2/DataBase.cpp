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

#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>

#include "DataBase.h"
#include "FileLocations.h"
#include "net/SimpleID.h"
#include "SimpleJSon.h"
#include "Storage.h"

DataBase::DataBase(QObject *parent)
  : QObject(parent)
{
}


/*!
 * Поиск пользователя по уникальному идентификатору.
 *
 * \param id Идентификатор пользователя или идентификатор cookie.
 * \return Указатель на пользователя или 0 если он не был найден.
 */
ChatUser DataBase::user(const QByteArray &id)
{
  qint64 key = userKey(id);
  if (key == -1)
    return ChatUser();

  return user(key);
}


/*!
 * Поиск пользователя по ключу в таблице users.
 *
 * \param id Ключ в таблице users.
 * \return Указатель на пользователя или 0 если он не был найден.
 */
ChatUser DataBase::user(qint64 id)
{
  QSqlQuery query;
  query.prepare(QLatin1String("SELECT userId, cookie, groups, nick, normalNick, gender, host, userAgent FROM users WHERE id = ? LIMIT 1;"));
  query.addBindValue(id);
  query.exec();

  if (!query.first())
    return ChatUser();

  ChatUser user(new ServerUser(query.value(0).toByteArray()));
  user->setKey(id);
  user->setCookie(query.value(1).toByteArray());
  user->setGroups(query.value(2).toString());
  user->setNick(query.value(3).toString());
  user->setNormalNick(query.value(4).toString());
  user->setRawGender(query.value(5).toInt());
  user->setHost(query.value(6).toString());
  user->setUserAgent(query.value(7).toString());

  return user;
}


int DataBase::start()
{
  QSqlDatabase db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"));
  QDir dir(Storage::i()->locations()->path(FileLocations::VarPath));
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());

  db.setDatabaseName(dir.absolutePath() + QLatin1String("/") + Storage::i()->locations()->path(FileLocations::BaseName) + QLatin1String(".sqlite"));
  if (!db.open())
    return -1;

  QSqlQuery query;
  query.exec(QLatin1String("PRAGMA synchronous = OFF"));
  QStringList tables = db.tables();

  if (!tables.contains(QLatin1String("users"))) {
    query.exec(QLatin1String(
    "CREATE TABLE users ( "
    "  id         INTEGER PRIMARY KEY,"
    "  userId     BLOB    NOT NULL UNIQUE,"
    "  cookie     BLOB    NOT NULL UNIQUE,"
    "  groups     TEXT,"
    "  nick       TEXT,"
    "  normalNick TEXT,"
    "  gender     INTEGER DEFAULT ( 0 ),"
    "  host       TEXT,"
    "  userAgent  TEXT"
    ");"));
  }

  if (!tables.contains(QLatin1String("groups"))) {
    query.exec(QLatin1String(
    "CREATE TABLE groups ( "
    "  id      INTEGER PRIMARY KEY,"
    "  name    TEXT    NOT NULL UNIQUE ,"
    "  allow   INTEGER DEFAULT ( 0 ),"
    "  deny    INTEGER DEFAULT ( 0 )"
    ");"));

    addGroup(QLatin1String("master"));
    addGroup(QLatin1String("regular"));
  }

  if (!tables.contains(QLatin1String("channels"))) {
    query.exec(QLatin1String(
    "CREATE TABLE channels ( "
    "  id         INTEGER PRIMARY KEY,"
    "  channelId  BLOB    NOT NULL UNIQUE,"
    "  name       TEXT,"
    "  normalName TEXT,"
    "  expired    INTEGER DEFAULT ( 0 ),"
    "  topic      TEXT,"
    "  data       TEXT"
    ");"));

    Storage::i()->channel(QLatin1String("Main"));
  }

  if (!tables.contains(QLatin1String("accounts"))) {
    query.exec(QLatin1String(
    "CREATE TABLE accounts ( "
    "  id       INTEGER PRIMARY KEY,"
    "  userId   BLOB    NOT NULL UNIQUE,"
    "  name     TEXT    NOT NULL UNIQUE,"
    "  password BLOB"
    ");"));
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
  qint64 key = userKey(user->id());
  if (key != -1) {
    user->setKey(key);

    if (user->cookie().isEmpty()) {
      QSqlQuery query;
      query.prepare(QLatin1String("SELECT cookie FROM users WHERE id = :id LIMIT 1;"));
      query.bindValue(QLatin1String(":id"), key);
      query.exec();

      if (query.first())
        user->setCookie(query.value(0).toByteArray());
    }

    update(user);
    return key;
  }

  if (user->cookie().isEmpty())
    user->setCookie(Storage::i()->cookie());

  QSqlQuery query;
  query.prepare(QLatin1String("INSERT INTO users (userId, cookie, groups, nick, normalNick, gender, host, userAgent) "
                     "VALUES (:userId, :cookie, :groups, :nick, :normalNick, :gender, :host, :userAgent);"));

  query.bindValue(QLatin1String(":userId"),     user->id());
  query.bindValue(QLatin1String(":cookie"),     user->cookie());
  query.bindValue(QLatin1String(":groups"),     user->groups().join(QLatin1String(",")));
  query.bindValue(QLatin1String(":nick"),       user->nick());
  query.bindValue(QLatin1String(":normalNick"), user->normalNick());
  query.bindValue(QLatin1String(":gender"),     user->rawGender());
  query.bindValue(QLatin1String(":host"),       user->host());
  query.bindValue(QLatin1String(":userAgent"),  user->userAgent());
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  key = query.lastInsertId().toLongLong();
  user->setKey(key);

  return key;
}


qint64 DataBase::addGroup(const QString &name, qint64 allow, qint64 deny)
{
  QSqlQuery query;
  query.prepare(QLatin1String("INSERT INTO groups (name, allow, deny) VALUES (:name, :allow, :deny);"));
  query.bindValue(QLatin1String(":name"), name);
  query.bindValue(QLatin1String(":allow"), allow);
  query.bindValue(QLatin1String(":deny"), deny);
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  return query.lastInsertId().toLongLong();
}


qint64 DataBase::userKey(const QByteArray &id)
{
  QSqlQuery query;

  int type = SimpleID::typeOf(id);
  if (type == SimpleID::UserId)
    query.prepare(QLatin1String("SELECT id FROM users WHERE userId = ? LIMIT 1;"));
  else if (type == SimpleID::CookieId)
    query.prepare(QLatin1String("SELECT id FROM users WHERE cookie = ? LIMIT 1;"));
  else
    return -1;

  query.addBindValue(id);
  query.exec();

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
  query.prepare(QLatin1String("UPDATE users SET cookie = :cookie, groups = :groups, nick = :nick, normalNick = :normalNick, gender = :gender, host = :host, userAgent = :userAgent WHERE id = :id;"));
  query.bindValue(QLatin1String(":cookie"), user->cookie());
  query.bindValue(QLatin1String(":groups"), user->groups().join(QLatin1String(",")));
  query.bindValue(QLatin1String(":nick"), user->nick());
  query.bindValue(QLatin1String(":normalNick"), user->normalNick());
  query.bindValue(QLatin1String(":gender"), user->rawGender());
  query.bindValue(QLatin1String(":host"), user->host());
  query.bindValue(QLatin1String(":userAgent"), user->userAgent());
  query.bindValue(QLatin1String(":id"), user->key());
  query.exec();
}


ChatChannel DataBase::channel(const QByteArray &id)
{
  qint64 key = channelKey(id);
  if (key == -1)
    return ChatChannel();

  return channel(key);
}


ChatChannel DataBase::channel(qint64 id)
{
  QSqlQuery query;
  query.prepare(QLatin1String("SELECT channelId, name, normalName, expired, topic, data FROM channels WHERE id = ? LIMIT 1;"));
  query.addBindValue(id);
  query.exec();

  if (!query.first())
    return ChatChannel();

  ChatChannel channel(new ServerChannel(query.value(0).toByteArray(), query.value(2).toString(), query.value(1).toString()));
  channel->setKey(id);
  channel->setData(SimpleJSon::parse(query.value(5).toByteArray()).toMap());
  channel->setTopic(query.value(4).toString());

  return channel;
}


qint64 DataBase::add(ChatChannel channel)
{
  qint64 key = channelKey(channel->id());
  if (key != -1) {
    channel->setKey(key);
    update(channel);
    return key;
  }

  QSqlQuery query;
  query.prepare(QLatin1String("INSERT INTO channels (channelId, name, normalName, expired, topic, data) "
                     "VALUES (:channelId, :name, :normalName, :expired, :topic, :data);"));

  query.bindValue(QLatin1String(":channelId"),  channel->id());
  query.bindValue(QLatin1String(":name"),       channel->name());
  query.bindValue(QLatin1String(":normalName"), channel->normalName());
  query.bindValue(QLatin1String(":expired"),    0);
  query.bindValue(QLatin1String(":topic"),      channel->topic().topic);
  query.bindValue(QLatin1String(":data"),       SimpleJSon::generate(channel->data()));
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  key = query.lastInsertId().toLongLong();
  channel->setKey(key);

  return key;
}


qint64 DataBase::channelKey(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::ChannelId)
    return -1;

  QSqlQuery query;
  query.prepare(QLatin1String("SELECT id FROM channels WHERE channelId = ? LIMIT 1;"));
  query.addBindValue(id);
  query.exec();

  if (!query.first())
    return -1;

  return query.value(0).toLongLong();
}


void DataBase::update(ChatChannel channel)
{
  QSqlQuery query;
  query.prepare(QLatin1String("UPDATE channels SET channelId = :channelId, name = :name, normalName = :normalName, expired = :expired, topic = :topic, data = :data WHERE id = :id;"));
  query.bindValue(QLatin1String(":channelId"), channel->id());
  query.bindValue(QLatin1String(":name"), channel->name());
  query.bindValue(QLatin1String(":normalName"), channel->normalName());
  query.bindValue(QLatin1String(":expired"), 0);
  query.bindValue(QLatin1String(":topic"), channel->topic().topic);
  query.bindValue(QLatin1String(":data"), SimpleJSon::generate(channel->data()));
  query.bindValue(QLatin1String(":id"), channel->key());
  query.exec();
}
