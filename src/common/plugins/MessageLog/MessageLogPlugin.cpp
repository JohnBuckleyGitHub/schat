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

#include "cores/Core.h"
#include "FileLocations.h"
#include "MessageLogPlugin.h"
#include "MessageLogPlugin_p.h"
#include "net/packets/message.h"
#include "net/packets/message.h"
#include "net/packets/users.h"
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
  else if (data.type() == NodeHook::UserReady) {
    userReady(static_cast<const UserReadyHook &>(data));
  }

  return HookResult(1);
}


QList<NodeHook::Type> MessageLog::hooks() const
{
  QList<NodeHook::Type> out;
  out += NodeHook::AcceptedMessage;
  out += NodeHook::UserReady;

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
  query.prepare(QLatin1String("INSERT INTO messages (messageId, senderId, destId, status, timestamp, command, text) "
                     "VALUES (:messageId, :senderId, :destId, :status, :timestamp, :command, :text);"));

  query.bindValue(QLatin1String(":messageId"), msg->id);
  query.bindValue(QLatin1String(":senderId"),  msg->senderId);
  query.bindValue(QLatin1String(":destId"),    msg->destId());
  query.bindValue(QLatin1String(":status"),    data.status());
  query.bindValue(QLatin1String(":timestamp"), data.timestamp());
  query.bindValue(QLatin1String(":command"),   msg->command);
  query.bindValue(QLatin1String(":text"),      msg->text);
  query.exec();
}


void MessageLog::cleanup(const QByteArray &destId)
{
  if (!m_offlineLog)
    return;

  QSqlQuery query(QSqlDatabase::database(m_id));
  if (m_privateLog)
    query.prepare(QLatin1String("UPDATE messages SET status = 0 WHERE destId = :destId AND status = 85;"));
  else
    query.prepare(QLatin1String("DELETE FROM messages WHERE destId = :destId AND status = 85;"));

  query.bindValue(QLatin1String(":destId"), destId);
  query.exec();
}


/*!
 * Запуск оффлайновой доставки сообщений.
 */
void MessageLog::offlineDelivery(const UserReadyHook &data)
{
  QSqlQuery query(QSqlDatabase::database(m_id));
  QByteArray id = data.user->id();

  query.prepare(QLatin1String("SELECT messageId, senderId, timestamp, command, text FROM messages WHERE destId = :destId AND status = 85;"));
  query.bindValue(QLatin1String(":destId"), id);
  query.exec();

  if (!query.isActive())
    return;

  QList<QByteArray> packets;
  QDataStream *stream = m_core->sendStream();
  QList<QByteArray> users = Storage::i()->users(id);

  // Цикл формирующий пакеты.
  while (query.next()) {
    QByteArray sender = query.value(1).toByteArray();

    // Если получателю не известны данные отправителя, то ему будут отосланы данные пользователя.
    if (!users.contains(sender)) {
      users.append(sender);
      ChatUser user = Storage::i()->user(sender, true);
      if (user)
        packets.append(UserWriter(stream, user.data(), id, UserWriter::StaticData).data());
    }

    // Формирование пакета с сообщением.
    MessageData msg(sender, id, query.value(3).toString(), query.value(4).toString());
    msg.flags = MessageData::OfflineFlag;
    msg.timestamp = query.value(2).toULongLong();
    msg.id = query.value(0).toByteArray();
    msg.autoSetOptions();

    packets.append(MessageWriter(stream, msg).data());
  }

  if (!packets.isEmpty())
    m_core->send(data.user, packets);

  cleanup(id);
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
    "  messageId  BLOB,"
    "  senderId   BLOB,"
    "  destId     BLOB,"
    "  status     INTEGER DEFAULT ( 0 ),"
    "  timestamp  INTEGER,"
    "  command    TEXT,"
    "  text       TEXT"
    ");"));

  m_isOpen = true;
}


void MessageLog::userReady(const UserReadyHook &data)
{
  if (m_offlineLog && (data.type == QLatin1String("restore") || data.type == QLatin1String("setup"))) {
    offlineDelivery(data);
    return;
  }

}


NodePlugin *MessageLogPlugin::init(Core *core)
{
  m_plugin = new MessageLog(core);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(MessageLog, MessageLogPlugin);
