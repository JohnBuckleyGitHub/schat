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
#include "client/ChatClient.h"
#include "client/SimpleClient.h"
#include "HistoryDB.h"
#include "text/PlainTextFilter.h"
#include "ui/tabs/ChatView.h"
#include "ui/tabs/PrivateTab.h"

HistoryDB::HistoryDB(QObject *parent)
  : QObject(parent)
{
  connect(ChatClient::io(), SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int)));

  m_lastMessages = ChatCore::settings()->value("History/LastMessages", 10).toInt();
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
    "  messageId  BLOB NOT NULL UNIQUE,"
    "  senderId   BLOB NOT NULL,"
    "  destId     BLOB NOT NULL,"
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
  Q_UNUSED(channelId)
//  ClientChannel channel = ChatCore::i()->client()->channel(channelId);
//  if (!channel)
//    return;
//
//  QSqlQuery query(QSqlDatabase::database(m_id));
//
//  query.prepare(QLatin1String("INSERT INTO channels (channelId, name) "
//                     "VALUES (:channelId, :name);"));
//
//  query.bindValue(QLatin1String(":channelId"), channelId);
//  query.bindValue(QLatin1String(":name"), channel->name());
//  query.exec();
}


qint64 HistoryDB::messageId(const QByteArray &id) const
{
  QSqlQuery query(QSqlDatabase::database(m_id));

  query.prepare(QLatin1String("SELECT id FROM messages WHERE messageId = :messageId LIMIT 1;"));
  query.bindValue(":messageId", id);
  query.exec();

  if (query.first())
    return query.value(0).toLongLong();

  return -1;
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
