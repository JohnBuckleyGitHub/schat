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
#include <QtPlugin>
#include <QVariant>

#include "FileLocations.h"
#include "MessageLogPlugin.h"
#include "MessageLogPlugin_p.h"
#include "net/packets/message.h"
#include "Storage.h"

MessageLog::MessageLog(Core *core)
  : NodePlugin(core)
  , m_isOpen(false)
  , m_logPrivate(true)
  , m_logPublic(true)
  , m_id(QLatin1String("messages"))
{
}


HookResult MessageLog::hook(const NodeHook &data)
{
  if (!m_isOpen)
    open();

  if (data.type() == NodeHook::AcceptedMessage)
    add(static_cast<const MessageHook &>(data));

  return HookResult(1);
}


QList<NodeHook::Type> MessageLog::hooks() const
{
  QList<NodeHook::Type> out;
  out += NodeHook::AcceptedMessage;

  return out;
}


void MessageLog::add(const MessageHook &data)
{
  MessageData *msg = data.data();
  if (msg->dest.size() > 1)
    return;

  if (SimpleID::typeOf(data.data()->destId()) == SimpleID::ChannelId && !m_logPublic)
    return;

  if (SimpleID::typeOf(data.data()->destId()) == SimpleID::UserId && !m_logPrivate)
    return;

  QSqlQuery query(QSqlDatabase::database(m_id));
  query.prepare(QLatin1String("INSERT INTO messages (senderId, destId, status, timestamp, command, text) "
                     "VALUES (:senderId, :destId, :status, :timestamp, :command, :text);"));

  query.bindValue(QLatin1String(":senderId"),  msg->senderId);
  query.bindValue(QLatin1String(":destId"),    msg->destId());
  query.bindValue(QLatin1String(":status"),    data.status());
  query.bindValue(QLatin1String(":timestamp"), data.timestamp());
  query.bindValue(QLatin1String(":command"),   msg->command);
  query.bindValue(QLatin1String(":text"),      msg->text);
  query.exec();
}


/*!
 * Открытие базы данных.
 */
void MessageLog::open()
{
  QSqlDatabase db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), m_id);
  db.setDatabaseName(Storage::i()->locations()->path(FileLocations::VarPath) + QLatin1String("/messages.sqlite"));
  if (!db.open())
    return;

  QSqlQuery query(db);
  query.exec(QLatin1String("PRAGMA synchronous = OFF"));

  query.exec(QLatin1String(
    "CREATE TABLE IF NOT EXISTS messages ( "
    "  id         INTEGER PRIMARY KEY,"
    "  senderId   BLOB,"
    "  destId     BLOB,"
    "  status     INTEGER DEFAULT ( 0 ),"
    "  timestamp  INTEGER,"
    "  command    TEXT,"
    "  text       TEXT"
    ");"));

  m_isOpen = true;
}


NodePlugin *MessageLogPlugin::init(Core *core)
{
  m_plugin = new MessageLog(core);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(MessageLog, MessageLogPlugin);
