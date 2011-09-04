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
#include <QSqlError>

#include "cores/Core.h"
#include "FileLocations.h"
#include "MessageLogPlugin.h"
#include "MessageLogPlugin_p.h"
#include "net/packets/message.h"
#include "net/packets/message.h"
#include "Settings.h"
#include "Storage.h"

MessageLog::MessageLog(Core *core)
  : NodePlugin(core)
  , m_isOpen(false)
  , m_offlineLog(true)
  , m_privateLog(false)
  , m_publicLog(true)
  , m_id(QLatin1String("messages"))
  , m_settings(Storage::i()->settings())
{
  m_settings->setDefault(QLatin1String("MessageLog/OfflineLog"), true);
  m_settings->setDefault(QLatin1String("MessageLog/PrivateLog"), false);
  m_settings->setDefault(QLatin1String("MessageLog/PublicLog"), true);
  reload();
}


HookResult MessageLog::hook(const NodeHook &data)
{
  if (!m_isOpen)
    open();

  if (data.type() == NodeHook::AcceptedMessage)
    add(static_cast<const MessageHook &>(data));
  else if (data.type() == NodeHook::AcceptedUser && m_offlineLog) {
    add(static_cast<const UserHook &>(data));
  }

  return HookResult(1);
}


QList<NodeHook::Type> MessageLog::hooks() const
{
  QList<NodeHook::Type> out;
  out += NodeHook::AcceptedMessage;
  out += NodeHook::AcceptedUser;

  if (m_offlineLog)
    out += NodeHook::OfflineDelivery;

  return out;
}


void MessageLog::reload()
{
  m_offlineLog = m_settings->value(QLatin1String("MessageLog/OfflineLog")).toBool();
  m_privateLog = m_settings->value(QLatin1String("MessageLog/PrivateLog")).toBool();
  m_publicLog  = m_settings->value(QLatin1String("MessageLog/PublicLog")).toBool();
}


/*!
 * Добавление сообщения в историю.
 */
void MessageLog::add(const MessageHook &data)
{
  MessageData *msg = data.data();
  if (msg->dest.size() > 1)
    return;

  if (SimpleID::typeOf(data.data()->destId()) == SimpleID::ChannelId && !m_publicLog)
    return;

  if (SimpleID::typeOf(data.data()->destId()) == SimpleID::UserId) {
    if (data.status() && !m_offlineLog)
      return;

    if (data.status() == 0 && !m_privateLog)
      return;
  }

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


void MessageLog::add(const UserHook &data)
{
  qDebug() << "uuuuuuuuuuuuuuuuuuuuuuuuuuuuuu";
  qDebug() << data.user->nick();
  qDebug() << "uuuuuuuuuuuuuuuuuuuuuuuuuuuuuu";

  QSqlQuery query(QSqlDatabase::database(m_id));

  query.prepare(QLatin1String("SELECT senderId, destId, timestamp, command, text FROM messages WHERE destId = :destId AND status = 1;"));
  query.bindValue(QLatin1String(":destId"), data.user->id());
  query.exec();

  qDebug() << query.lastError();

  if (!query.isActive())
    return;

  QList<QByteArray> packets;
  qDebug() << m_core;
  QDataStream *stream = m_core->sendStream();

  while (query.next()) {
    MessageData msg(query.value(0).toByteArray(), query.value(1).toByteArray(), query.value(3).toString(), query.value(4).toString());
    msg.flags = MessageData::OfflineFlag;
    msg.name = query.value(2).toULongLong();
    packets.append(MessageWriter(stream, msg).data());
  }

//  if (!packets.isEmpty())
//    m_core->send(data.user, packets);
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
