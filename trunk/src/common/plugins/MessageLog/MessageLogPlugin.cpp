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
#include "net/packets/Notice.h"
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


void MessageLog::cleanup(const QByteArray &destId)
{
  if (!m_offlineLog)
    return;

  QSqlQuery query(QSqlDatabase::database(m_id));
  if (m_privateLog)
    query.prepare(QLatin1String("UPDATE messages SET status = 200 WHERE destId = :destId AND status = 407;"));
  else
    query.prepare(QLatin1String("DELETE FROM messages WHERE destId = :destId AND status = 407;"));

  query.bindValue(QLatin1String(":destId"), destId);
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
    "  messageId  BLOB,"
    "  senderId   BLOB,"
    "  destId     BLOB,"
    "  status     INTEGER DEFAULT ( 200 ),"
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
