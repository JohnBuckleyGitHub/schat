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

#include "Channel.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/SimpleClient.h"
#include "HistoryDB.h"
#include "HistoryUserMessage.h"
#include "net/packets/message.h"
#include "ui/tabs/ChatView.h"
#include "ui/tabs/PrivateTab.h"
#include "ui/UserUtils.h"
#include "User.h"

HistoryDB::HistoryDB(QObject *parent)
  : QObject(parent)
{
  connect(ChatCore::i()->client(), SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int)));
  connect(ChatCore::i()->client(), SIGNAL(userDataChanged(const QByteArray &)), SLOT(updateUserData(const QByteArray &)));
  connect(ChatCore::i()->client(), SIGNAL(userLeave(const QByteArray &)), SLOT(updateUserData(const QByteArray &)));
  connect(ChatCore::i()->client(), SIGNAL(synced(const QByteArray &)), SLOT(synced(const QByteArray &)));

  m_lastMessages = ChatCore::i()->settings()->value("History/LastMessages", 10).toInt();
}


ClientUser HistoryDB::user(const QByteArray &id) const
{
  qint64 index = userId(id);
  if (index == -1)
    return ClientUser();

  QSqlQuery query(QSqlDatabase::database(m_id));

  query.prepare(QLatin1String("SELECT nick, gender, host, userAgent FROM users WHERE id = ? LIMIT 1;"));
  query.addBindValue(index);
  query.exec();

  if (!query.first())
    return ClientUser();

  ClientUser user(new User());
  user->setId(id);
  user->setNick(query.value(0).toString());
  user->setRawGender(query.value(1).toULongLong());
  user->setHost(query.value(2).toString());
  user->setUserAgent(query.value(3).toString());

  return user;
}


/*!
 * Добавление сообщения в историю.
 */
qint64 HistoryDB::add(int status, const MessageData &data)
{
  if (addUser(data.senderId) == -1)
    return -1;

  if (SimpleID::typeOf(data.destId) == SimpleID::UserId)
    addUser(data.destId);

  QSqlQuery query(QSqlDatabase::database(m_id));

  query.prepare(QLatin1String("INSERT INTO messages (senderId, destId, status, timestamp, command, text, plainText) "
                     "VALUES (:senderId, :destId, :status, :timestamp, :command, :text, :plainText);"));

  query.bindValue(QLatin1String(":senderId"), data.senderId);
  query.bindValue(QLatin1String(":destId"), data.destId);
  query.bindValue(QLatin1String(":status"), status);
  query.bindValue(QLatin1String(":timestamp"), data.timestamp);
  query.bindValue(QLatin1String(":command"), data.command);
  query.bindValue(QLatin1String(":text"), data.text);
  query.bindValue(QLatin1String(":plainText"), MessageUtils::toPlainText(data.text));
  query.exec();

  if (query.numRowsAffected() <= 0)
    return -1;

  query.exec(QLatin1String("SELECT last_insert_rowid();"));
  if (!query.first())
    return -1;

  return query.value(0).toLongLong();
}


/*!
 * Добавление пользователя в базу данных.
 */
qint64 HistoryDB::addUser(const QByteArray &id)
{
  ClientUser user = UserUtils::user(id);
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
  m_cache[id] = result;
  return result;
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

  qint64 index = userId(id);
  if (index == -1)
    return -1;

  QSqlQuery query(QSqlDatabase::database(m_id));

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
 * Загрузка последних сообщений в приватный разговор.
 */
void HistoryDB::loadLast(PrivateTab *tab)
{
  QByteArray senderId = tab->id();
  qint64 index = userId(senderId);
  if (index == -1)
    return;

  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(QLatin1String("SELECT * FROM (SELECT id, senderId, destId, status, timestamp, command, text FROM messages WHERE destId = :destId OR senderId = :senderId AND destId = :myId ORDER BY id DESC LIMIT ")
      + QString::number(m_lastMessages) + QLatin1String(") ORDER BY id;"));

  query.bindValue(QLatin1String(":destId"), senderId);
  query.bindValue(QLatin1String(":senderId"), senderId);
  query.bindValue(QLatin1String(":myId"), UserUtils::userId());
  query.exec();

  if (!query.isActive())
    return;

  ClientUser user = UserUtils::user(senderId);
  if (!user)
    return;

  while (query.next()) {
    QByteArray id;
    MessageData data(query.value(1).toByteArray(), query.value(2).toByteArray(), query.value(5).toString(), query.value(6).toString());
    data.timestamp = query.value(4).toULongLong();

    HistoryUserMessage msg(query.value(3).toULongLong(), data);
    tab->chatView()->evaluateJavaScript(msg.js());
  }
}


/*!
 * Открытие базы данных.
 *
 * Причина использования "PRAGMA synchronous = OFF": https://plus.google.com/109693334715857802924/posts/jEmzo7JRkWy
 * Если база уже была открыта, предыдущее соединение закрывается.
 * В случае необходимости создаются таблицы:
 * - users
 * - channels
 * - messages
 *
 * \param id  Идентификатор сервера, используется для формирования имени соединения с базой.
 * \param dir Папка, в которой находится база.
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
    "  senderId   BLOB,"
    "  destId     BLOB,"
    "  status     INTEGER,"
    "  timestamp  INTEGER,"
    "  command    TEXT,"
    "  text       TEXT,"
    "  plainText  TEXT"
    ");"));
}


/*!
 * Обработка изменения состояния клиента, в случае перехода
 * в состояние "В сети" очищается кэш.
 */
void HistoryDB::clientStateChanged(int state)
{
  if (state == SimpleClient::ClientOnline)
    m_cache.clear();
}


void HistoryDB::synced(const QByteArray &channelId)
{
  ClientChannel channel = ChatCore::i()->client()->channel(channelId);
  if (!channel)
    return;

  QSqlQuery query(QSqlDatabase::database(m_id));

  query.prepare(QLatin1String("INSERT INTO channels (channelId, name) "
                     "VALUES (:channelId, :name);"));

  query.bindValue(QLatin1String(":channelId"), channelId);
  query.bindValue(QLatin1String(":name"), channel->name());
  query.exec();
}


/*!
 * Обновление информации о пользователе, в случае если он находится в кэше.
 */
void HistoryDB::updateUserData(const QByteArray &userId)
{
  updateUser(userId);
}


qint64 HistoryDB::userId(const QByteArray &id) const
{
  if (m_cache.contains(id))
    return m_cache.value(id);

  QSqlQuery query(QSqlDatabase::database(m_id));

  query.prepare(QLatin1String("SELECT id FROM users WHERE userId = ? LIMIT 1;"));
  query.addBindValue(id);
  query.exec();

  if (query.first())
    return query.value(0).toLongLong();
  else
    return -1;
}


/*!
 * Закрытие базы данных и очистка кэша.
 */
void HistoryDB::close()
{
  m_cache.clear();
  QSqlDatabase::removeDatabase(m_id);
}
