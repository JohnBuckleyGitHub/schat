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

#include <QCoreApplication>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>

#include "DataBase.h"
#include "FileLocations.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "SimpleJSon.h"
#include "Storage.h"

bool Account::isValid() const
{
  if (id == -1)
    return false;

  if (name.isEmpty())
    return false;

  if (SimpleID::typeOf(userId) != SimpleID::UserId)
    return false;

  if (SimpleID::typeOf(password) != SimpleID::PasswordId)
    return false;

  return true;
}


DataBase::DataBase(QObject *parent)
  : QObject(parent)
{
}


int DataBase::start()
{
  QSqlDatabase db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"));
  QDir dir(Storage::i()->locations()->path(FileLocations::VarPath));
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());

  db.setDatabaseName(dir.absolutePath() + QLatin1String("/") + Storage::i()->locations()->path(FileLocations::BaseName) + QLatin1String(".sqlite"));
  if (!db.open()) {
    SCHAT_LOG_FATAL() << "Could not open DataBase file" << db.databaseName() << ":" << db.lastError();
    QCoreApplication::exit(-1);
    return -1;
  }

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
    "  account    TEXT,"
    "  userAgent  TEXT"
    ");"));
  }

  if (!tables.contains(QLatin1String("groups"))) {
    query.exec(QLatin1String(
    "CREATE TABLE groups ( "
    "  id          INTEGER PRIMARY KEY,"
    "  name        TEXT    NOT NULL UNIQUE ,"
    "  permissions TEXT"
    ");"));

    addGroup("master");
    addGroup("registered");
    addGroup("anonymous");
  }

  if (!tables.contains(QLatin1String("channels"))) {
    query.exec(QLatin1String(
    "CREATE TABLE channels ( "
    "  id         INTEGER PRIMARY KEY,"
    "  channelId  BLOB    NOT NULL UNIQUE,"
    "  name       TEXT,"
    "  expired    INTEGER DEFAULT ( 0 ),"
    "  feeds      TEXT,"
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
    "  password BLOB,"
    "  data     TEXT"
    ");"));
  }

  return 0;
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
  query.prepare(QLatin1String("SELECT userId, cookie, groups, nick, normalNick, gender, host, account, userAgent FROM users WHERE id = ? LIMIT 1;"));
  query.addBindValue(id);
  query.exec();

  if (!query.first())
    return ChatUser();

  ChatUser user(new ServerUser(query.value(0).toByteArray()));
  user->setKey(id);
  user->setCookie(query.value(1).toByteArray());
  user->groups().set(query.value(2).toString());
  user->setNick(query.value(3).toString());
  user->setNormalNick(query.value(4).toString());
  user->setRawGender(query.value(5).toInt());
  user->setHost(query.value(6).toString());
  user->setAccount(query.value(7).toString());
  user->setUserAgent(query.value(8).toString());

  return user;
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
  query.bindValue(QLatin1String(":groups"),     user->groups().toString());
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


qint64 DataBase::addGroup(const QString &name, const QString &permissions)
{
  QSqlQuery query;
  query.prepare(QLatin1String("INSERT INTO groups (name, permissions) VALUES (:name, :permissions);"));
  query.bindValue(QLatin1String(":name"), name);
  query.bindValue(QLatin1String(":permissions"), permissions);
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
  query.prepare(QLatin1String("UPDATE users SET cookie = :cookie, groups = :groups, nick = :nick, normalNick = :normalNick, gender = :gender, host = :host, account = :account, userAgent = :userAgent WHERE id = :id;"));
  query.bindValue(QLatin1String(":cookie"),     user->cookie());
  query.bindValue(QLatin1String(":groups"),     user->groups().toString());
  query.bindValue(QLatin1String(":nick"),       user->nick());
  query.bindValue(QLatin1String(":normalNick"), user->normalNick());
  query.bindValue(QLatin1String(":gender"),     user->rawGender());
  query.bindValue(QLatin1String(":host"),       user->host());
  query.bindValue(QLatin1String(":account"),    user->account());
  query.bindValue(QLatin1String(":userAgent"),  user->userAgent());
  query.bindValue(QLatin1String(":id"),         user->key());
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
  query.prepare(QLatin1String("SELECT channelId, name, expired, feeds, data FROM channels WHERE id = ? LIMIT 1;"));
  query.addBindValue(id);
  query.exec();

  if (!query.first())
    return ChatChannel();

  ChatChannel channel(new ServerChannel(query.value(0).toByteArray(), query.value(1).toString()));
  channel->setKey(id);
  channel->setData(SimpleJSon::parse(query.value(4).toByteArray()).toMap());
//  channel->setTopic(SimpleJSon::parse(query.value(3).toString().toUtf8()));

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
  query.prepare(QLatin1String("INSERT INTO channels (channelId, name, expired, feeds, data) "
                     "VALUES (:channelId, :name, :expired, :feeds, :data);"));

  query.bindValue(QLatin1String(":channelId"),  channel->id());
  query.bindValue(QLatin1String(":name"),       channel->name());
  query.bindValue(QLatin1String(":expired"),    0);
  query.bindValue(QLatin1String(":feeds"),      SimpleJSon::generate(Feeds::merge("feeds", channel->feeds().headers())));
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
  query.prepare(QLatin1String("UPDATE channels SET channelId = :channelId, name = :name, expired = :expired, feeds = :feeds, data = :data WHERE id = :id;"));
  query.bindValue(QLatin1String(":channelId"), channel->id());
  query.bindValue(QLatin1String(":name"), channel->name());
  query.bindValue(QLatin1String(":expired"), 0);
  query.bindValue(QLatin1String(":feeds"), SimpleJSon::generate(Feeds::merge("feeds", channel->feeds().headers())));
  query.bindValue(QLatin1String(":data"), SimpleJSon::generate(channel->data()));
  query.bindValue(QLatin1String(":id"), channel->key());
  query.exec();
}


/*!
 * Получение ключа в таблице \p accounts для имени аккаунта пользователя.
 *
 * \param name Аккаунт пользователя вида.
 * \return Объект Account.
 */
Account DataBase::account(const QString &name) const
{
  QSqlQuery query;
  query.prepare("SELECT id, userId, password, data FROM accounts WHERE name = :name LIMIT 1;");
  query.bindValue(":name", name);
  query.exec();

  if (!query.first())
    return Account();

  Account account;
  account.id = query.value(0).toLongLong();
  account.userId = query.value(1).toByteArray();
  account.name = name;
  account.password = query.value(2).toByteArray();
  account.data = SimpleJSon::parse(query.value(3).toString().toUtf8());

  return account;
}


/*!
 * Регистрация пользователя.
 * Эта функция не проверяет корректность имени аккаунта и пароля.
 *
 * \param user     Пользователь.
 * \param name     Имя аккаунта пользователя.
 * \param password Пароль.
 * \param data     JSON данные.
 *
 * \return Возвращает ключ в таблице \p accounts или -1 если произошла ошибка при вставке в таблицу или -2 если аккаунт уже зарегистрирован.
 */
qint64 DataBase::reg(ChatUser user, const QString &name, const QByteArray &password, const QVariant &data)
{
  if (account(name).id != -1)
    return -2;

  QSqlQuery query;
  query.prepare(QLatin1String("INSERT INTO accounts (userId, name, password, data) VALUES (:userId, :name, :password, :data);"));
  query.bindValue(":userId", user->id());
  query.bindValue(":name", name);
  query.bindValue(":password", password);
  query.bindValue(":data", SimpleJSon::generate(data));
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  return query.lastInsertId().toLongLong();
}
