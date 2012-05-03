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

#include <QCoreApplication>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>

#include "Ch.h"
#include "DataBase.h"
#include "DateTime.h"
#include "feeds/FeedStorage.h"
#include "FileLocations.h"
#include "JSON.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "sglobal.h"
#include "Storage.h"

bool DataBase::noMaster = false;

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

  if (!tables.contains(LS("accounts"))) {
    query.exec(LS(
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

  if (!tables.contains(LS("groups"))) {
    query.exec(LS(
    "CREATE TABLE groups ( "
    "  id          INTEGER PRIMARY KEY,"
    "  name        TEXT    NOT NULL UNIQUE ,"
    "  permissions TEXT"
    ");"));

    addGroup("master");
    addGroup("registered");
    addGroup("anonymous");
  }

  if (!tables.contains(LS("channels"))) {
    query.exec(LS(
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

    noMaster = true;
  }

  return 0;
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


bool DataBase::isCollision(const QByteArray &id, const QByteArray &normalized, int type)
{
  QSqlQuery query;
  query.prepare("SELECT channel FROM channels WHERE normalized = :normalized AND type = :type LIMIT 1;");
  query.bindValue(":normalized", normalized);
  query.bindValue(":type", type);
  query.exec();

  if (!query.first())
    return false;

  return query.value(0).toByteArray() != id;
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
  query.prepare(LS("SELECT channel, gender, status, account, name, data FROM channels WHERE id = :id LIMIT 1;"));
  query.bindValue(LS(":id"), id);
  query.exec();

  if (!query.first())
    return ChatChannel();

  ChatChannel channel(new ServerChannel(query.value(0).toByteArray(), query.value(4).toString()));
  channel->setKey(id);
  channel->gender().setRaw(query.value(1).toLongLong());

  qint64 accountId = query.value(3).toLongLong();
  if (accountId > 0) {
    Account account = DataBase::account(accountId);
    channel->setAccount(&account);
  }

  channel->setData(JSON::parse(query.value(5).toByteArray()).toMap());
  FeedStorage::load(channel.data());

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
  query.prepare("INSERT INTO channels (channel, normalized, type, gender, name, data) "
                     "VALUES (:channel, :normalized, :type, :gender, :name, :data);");

  query.bindValue(":channel",    channel->id());
  query.bindValue(":normalized", channel->normalized());
  query.bindValue(":type",       channel->type());
  query.bindValue(":gender",     channel->gender().raw());
  query.bindValue(":name",       channel->name());
  query.bindValue(":data",       JSON::generate(channel->data()));
  query.exec();

  if (query.numRowsAffected() <= 0) {
    SCHAT_LOG_ERROR() << "Could not add channel:" << query.lastError();
    return -1;
  }

  key = query.lastInsertId().toLongLong();
  channel->setKey(key);
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


QString DataBase::nick(qint64 id)
{
  QSqlQuery query;
  query.prepare(LS("SELECT name FROM channels WHERE id = :id LIMIT 1;"));
  query.bindValue(LS(":id"), id);
  query.exec();

  if (!query.first())
    return QString();

  return query.value(0).toString();
}


void DataBase::saveData(Channel *channel)
{
  QSqlQuery query;
  query.prepare("UPDATE channels SET data = :data WHERE id = :id;");
  query.bindValue(":data",       JSON::generate(channel->data()));
  query.bindValue(":id",         channel->key());
  query.exec();
}


/*!
 * Обновление информации об канале.
 * Если канал также содержит пользовательский аккаунт, то он также будет обновлён.
 */
void DataBase::update(ChatChannel channel)
{
  if (!channel)
    return;

  QSqlQuery query;
  query.prepare("UPDATE channels SET channel = :channel, normalized = :normalized, type = :type, gender = :gender, name = :name, data = :data WHERE id = :id;");
  query.bindValue(":channel",    channel->id());
  query.bindValue(":normalized", channel->normalized());
  query.bindValue(":type",       channel->type());
  query.bindValue(":gender",     channel->gender().raw());
  query.bindValue(":name",       channel->name());
  query.bindValue(":data",       JSON::generate(channel->data()));
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
    account->setCookie(Ch::cookie());

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

  query.prepare("UPDATE channels SET account = :account WHERE id = :id;");
  query.bindValue(":account", key);
  query.bindValue(":id", account->channel());
  query.exec();

  return key;
}
