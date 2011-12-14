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
#include "DateTime.h"
#include "FileLocations.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "SimpleJSon.h"
#include "Storage.h"

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

  if (!tables.contains(QLatin1String("accounts"))) {
    query.exec(QLatin1String(
    "CREATE TABLE accounts ( "
    "  id         INTEGER PRIMARY KEY,"
    "  channel    INTEGER DEFAULT ( 0 ),"
    "  date       INTEGER DEFAULT ( 0 ),"
    "  cookie     BLOB    NOT NULL UNIQUE,"
    "  name       TEXT,"
    "  password   BLOB,"
    "  groups     TEXT"
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
    "  channel    BLOB    NOT NULL UNIQUE,"
    "  normalized BLOB    NOT NULL UNIQUE,"
    "  type       INTEGER DEFAULT ( 73 ),"
    "  gender     INTEGER DEFAULT ( 0 ),"
    "  status     INTEGER DEFAULT ( 0 ),"
    "  account    INTEGER DEFAULT ( 0 ),"
    "  name       TEXT    NOT NULL,"
    "  data       BLOB"
    ");"));

    Storage::i()->channel(QLatin1String("Main"));
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
  user->gender().setRaw(query.value(5).toInt());
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
  query.bindValue(QLatin1String(":gender"),     user->gender().raw());
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
  query.bindValue(QLatin1String(":gender"),     user->gender().raw());
  query.bindValue(QLatin1String(":host"),       user->host());
  query.bindValue(QLatin1String(":account"),    user->account());
  query.bindValue(QLatin1String(":userAgent"),  user->userAgent());
  query.bindValue(QLatin1String(":id"),         user->key());
  query.exec();
}


/*!
 * Получение канала на основе идентификатора канала или нормализированного имени и типа канала.
 */
ChatChannel DataBase::channel(const QByteArray &id, int type)
{
  qint64 key = channelKey(id, type);
  if (key == -1)
    return ChatChannel();

  return channel(key);
}


/*!
 * Получение канала на основе первичного ключа в таблице \b channels.
 */
ChatChannel DataBase::channel(qint64 id)
{
  QSqlQuery query;
  query.prepare(QLatin1String("SELECT channel, gender, status, account, name, data FROM channels WHERE id = ? LIMIT 1;"));
  query.addBindValue(id);
  query.exec();

  if (!query.first())
    return ChatChannel();

  ChatChannel channel(new ServerChannel(query.value(0).toByteArray(), query.value(4).toString()));
  channel->setKey(id);
  channel->gender().setRaw(query.value(1).toLongLong());
  channel->status().set(query.value(2).toLongLong());

  qint64 accountId = query.value(3).toLongLong();
  if (accountId > 0) {
    Account account = DataBase::account(accountId);
    channel->setAccount(&account);
  }

  channel->setData(SimpleJSon::parse(query.value(5).toByteArray()).toMap());

  return channel;
}


/*!
 * Добавление канала.
 * Если канал уже добавлен, он будет обновлён.
 *
 * \param channel Указатель на канал.
 * \return Ключ в таблице \b channels или -1 в случае ошибки.
 *
 * \sa update().
 */
qint64 DataBase::add(ChatChannel channel)
{
  qint64 key = channelKey(channel->id(), channel->type());
  if (key != -1) {
    channel->setKey(key);
    update(channel);
    return key;
  }

  QSqlQuery query;
  query.prepare("INSERT INTO channels (channel, normalized, type, gender, status, name, data) "
                     "VALUES (:channel, :normalized, :type, :gender, :status, :name, :data);");

  query.bindValue(":channel",    channel->id());
  query.bindValue(":normalized", channel->normalized());
  query.bindValue(":type",       channel->type());
  query.bindValue(":gender",     channel->gender().raw());
  query.bindValue(":status",     channel->status().value());
  query.bindValue(":name",       channel->name());
  query.bindValue(":data",       SimpleJSon::generate(channel->feeds().save()));
  query.exec();

  if (query.numRowsAffected() <= 0) {
    SCHAT_LOG_ERROR() << "Could not add channel:" << query.lastError();
    return -1;
  }

  key = query.lastInsertId().toLongLong();
  channel->setKey(key);

  // Создание нового аккаунта.
  if (SimpleID::typeOf(channel->id()) == SimpleID::UserId && channel->account()) {
    channel->account()->setChannel(key);

    if (add(channel->account()) != -1) {
      query.prepare("UPDATE channels SET account = :account WHERE id = :id;");
      query.bindValue(":account", channel->account()->id());
      query.bindValue(":id", key);
      query.exec();
    }
  }

  return key;
}


/*!
 * Возвращает ключ в таблице \b channels на основе идентификатора канала или нормализированного имени.
 *
 * \param id   Идентификатор по которому будет производится поиск.
 * \param type Тип канала.
 *
 * \return Ключ в таблице или -1 в случае ошибки.
 *
 * \sa Normalize.
 * \sa SimpleID.
 */
qint64 DataBase::channelKey(const QByteArray &id, int type)
{
  if (SimpleID::typeOf(id) == SimpleID::CookieId) {
    qint64 key = accountKey(id);
    if (key == -1)
      return key;

    QSqlQuery query;
    query.prepare("SELECT channel FROM accounts WHERE id = :id LIMIT 1;");
    query.bindValue(":id", key);
    query.exec();

    if (!query.first())
      return -1;

    return query.value(0).toLongLong();
  }

  if (Channel::isCompatibleId(id) == 0 && SimpleID::typeOf(id) != SimpleID::NormalizedId)
    return -1;

  QSqlQuery query;

  if (SimpleID::typeOf(id) == SimpleID::NormalizedId) {
    query.prepare("SELECT id FROM channels WHERE normalized = :id AND type = :type LIMIT 1;");
  }
  else {
    query.prepare("SELECT id FROM channels WHERE channel = :id AND type = :type LIMIT 1;");
  }

  query.bindValue(":id",   id);
  query.bindValue(":type", type);
  query.exec();

  if (!query.first())
    return -1;

  return query.value(0).toLongLong();
}


/*!
 * Обновление информации об канале.
 * Если канал также содержит пользовательский аккаунт, то он также будет обновлён.
 */
void DataBase::update(ChatChannel channel)
{
  QSqlQuery query;
  query.prepare("UPDATE channels SET channel = :channel, normalized = :normalized, type = :type, gender = :gender, status = :status, name = :name, data = :data WHERE id = :id;");
  query.bindValue(":channel",    channel->id());
  query.bindValue(":normalized", channel->normalized());
  query.bindValue(":type",       channel->type());
  query.bindValue(":gender",     channel->gender().raw());
  query.bindValue(":status",     channel->status().value());
  query.bindValue(":name",       channel->name());
  query.bindValue(":data",       SimpleJSon::generate(channel->feeds().save()));
  query.bindValue(":id",         channel->key());
  query.exec();

  if (channel->account() && channel->account()->id() > 0) {
    channel->account()->setDate(DateTime::utc());

    query.prepare("UPDATE accounts SET date = :date, cookie = :cookie, name = :name, password = :password, groups = :groups WHERE id = :id;");
    query.bindValue(":date",       channel->account()->date());
    query.bindValue(":cookie",     channel->account()->cookie());
    query.bindValue(":name",       channel->account()->name());
    query.bindValue(":password",   channel->account()->password());
    query.bindValue(":groups",     channel->account()->groups().toString());
    query.bindValue(":id",         channel->account()->id());
    query.exec();
  }
}


/*!
 * Получение аккаунта пользователя.
 *
 * \param key Ключ в таблице \b accounts.
 */
Account DataBase::account(qint64 key)
{
  QSqlQuery query;
  query.prepare("SELECT channel, date, cookie, name, password, groups FROM accounts WHERE id = :id LIMIT 1;");
  query.bindValue(":id", key);
  query.exec();

  if (!query.first())
    return Account();

  Account account;
  account.setId(key);
  account.setChannel(query.value(0).toLongLong());
  account.setDate(query.value(1).toLongLong());
  account.setCookie(query.value(2).toByteArray());
  account.setName(query.value(3).toString());
  account.setPassword(query.value(4).toByteArray());
  account.groups().set(query.value(5).toString());

  return account;
}


/*!
 * Получение ключа в таблице \b accounts на основе Сookie пользователя.
 */
qint64 DataBase::accountKey(const QByteArray &cookie)
{
  if (SimpleID::typeOf(cookie) != SimpleID::CookieId)
    return -1;

  QSqlQuery query;
  query.prepare("SELECT id FROM accounts WHERE cookie = :cookie LIMIT 1;");
  query.bindValue(":cookie", cookie);
  query.exec();

  if (!query.first())
    return -1;

  return query.value(0).toLongLong();
}


/*!
 * Получение ключа в таблице \b accounts на основе зарегистрированного имени пользователя.
 */
qint64 DataBase::accountKey(const QString &name)
{
  if (name.isEmpty())
    return -1;

  QSqlQuery query;
  query.prepare("SELECT id FROM accounts WHERE name = :name LIMIT 1;");
  query.bindValue(":name", name);
  query.exec();

  if (!query.first())
    return -1;

  return query.value(0).toLongLong();
}


/*!
 * Получение ключа в таблице \b accounts на основе номера канала.
 */
qint64 DataBase::accountKey(qint64 channel)
{
  if (channel < 1)
    return -1;

  QSqlQuery query;
  query.prepare("SELECT id FROM accounts WHERE channel = :channel LIMIT 1;");
  query.bindValue(":channel", channel);
  query.exec();

  if (!query.first())
    return -1;

  return query.value(0).toLongLong();
}


qint64 DataBase::add(Account *account)
{
  if (!account->channel())
    return -1;

  if (account->cookie().isEmpty())
    account->setCookie(Storage::i()->cookie());

  QSqlQuery query;
  query.prepare("INSERT INTO accounts (channel, date, cookie, name, password, groups) "
                     "VALUES (:channel, :date, :cookie, :name, :password, :groups);");

  query.bindValue(":channel",    account->channel());
  query.bindValue(":date",       account->date());
  query.bindValue(":cookie",     account->cookie());
  query.bindValue(":name",       account->name());
  query.bindValue(":password",   account->password());
  query.bindValue(":groups",     account->groups().toString());
  query.exec();

  if (query.numRowsAffected() <= 0) {
    SCHAT_LOG_ERROR() << "Could not add account:" << query.lastError();
    return -1;
  }

  qint64 key = query.lastInsertId().toLongLong();
  account->setId(key);

  return key;
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
//  if (account(name).id() != -1)
//    return -2;
//
//  QSqlQuery query;
//  query.prepare(QLatin1String("INSERT INTO accounts (userId, name, password, data) VALUES (:userId, :name, :password, :data);"));
//  query.bindValue(":userId", user->id());
//  query.bindValue(":name", name);
//  query.bindValue(":password", password);
//  query.bindValue(":data", SimpleJSon::generate(data));
//  query.exec();
//
//  if (query.numRowsAffected() <= 0)
//    return -1;
//
//  return query.lastInsertId().toLongLong();
  return -1;
}
