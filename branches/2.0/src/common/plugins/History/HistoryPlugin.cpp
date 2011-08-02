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

#include <qplugin.h>

#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "HistoryDB.h"
#include "HistoryPlugin.h"
#include "HistoryPlugin_p.h"
#include "messages/UserMessage.h"
#include "net/packets/message.h"
#include "NetworkManager.h"
#include "plugins/hooks.h"

History::History(ChatCore *core)
  : ChatPlugin(core)
{
  m_db = new HistoryDB(this);
  openDb();
}


QList<HookData::Type> History::hooks() const
{
  QList<HookData::Type> out;
  out += HookData::RawUserMessage;

  return out;
}


void History::hook(const HookData &data)
{
  switch (data.type()) {
    case HookData::RawUserMessage:
      add(static_cast<const RawUserMessageHook &>(data));
      break;
    default:
      break;
  }
}


void History::notify(int notice, const QVariant &data)
{
  if (notice == ChatCore::NetworkChangedNotice) {
    openDb();
  }
}


void History::add(const RawUserMessageHook &data)
{
  /// FIXME Добавлять канал в историю.

  QString nick;
  ClientUser user = m_core->client()->user(data.data.senderId);
  if (user) {
    nick = user->nick();
    // FIXME Добавить добавление пользователя в историю в случае приватного разговора.
  }
  QTime t;
  t.start();
  qDebug() << m_db->add(data.status, data.data, nick, MessageUtils::toPlainText(data.data.text));
  qDebug() << "add time:" << t.elapsed() << "ms";
}


void History::openDb()
{
  QByteArray id = m_core->networks()->serverId();
  if (!id.isEmpty())
    m_db->open(id, m_core->networks()->root());
}


ChatPlugin *HistoryPlugin::init(ChatCore *core)
{
  return new History(core);
}

Q_EXPORT_PLUGIN2(History, HistoryPlugin);
