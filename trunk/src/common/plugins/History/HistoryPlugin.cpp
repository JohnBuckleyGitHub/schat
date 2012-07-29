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

#include <QTime>
#include <QTimer>
#include <QtPlugin>

#include "ChatCore.h"
#include "ChatNotify.h"
#include "client/ChatClient.h"
#include "client/ClientFeeds.h"
#include "HistoryChatView.h"
#include "HistoryDB.h"
#include "HistoryMessages.h"
#include "HistoryPlugin.h"
#include "HistoryPlugin_p.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "sglobal.h"
#include "text/MessageId.h"
#include "ui/tabs/PrivateTab.h"
#include "client/ClientMessages.h"

History::History(QObject *parent)
  : ChatPlugin(parent)
{
  m_chatView = new HistoryChatView(this);
  new HistoryMessages(this);

  open();
  connect(ChatClient::i(), SIGNAL(online()), SLOT(open()));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
}


/*!
 * Загрузка сообщений по идентификаторам.
 */
bool History::get(const QList<MessageId> &ids)
{
  QList<MessageId> required = getLocal(ids);
  if (required.isEmpty())

  if (ChatClient::state() != ChatClient::Online)
    return false;

  QVariantMap data;
  data.insert(LS("action"), LS("get"));
  data.insert(LS("ids"), MessageId::toString(required));

  return ChatClient::feeds()->request(ChatClient::id(), LS("query"), LS("history"), data);
}


/*!
 * Отправка пакета с запросом на получение последних сообщений в канале.
 *
 * \param id Идентификатор канала.
 */
bool History::getLast(const QByteArray &id)
{
  if (ChatClient::state() != ChatClient::Online) {
    getLocal(HistoryDB::last(id, 20));
    return false;
  }

  QVariantMap data;
  data.insert(LS("action"), LS("last"));
  data.insert(LS("count"), 20);

  QByteArray channel = id;
  if (SimpleID::typeOf(id) == SimpleID::UserId && ChatClient::id() != id) {
    data.insert(LS("id"), SimpleID::encode(id));
    channel = ChatClient::id();
  }

  return ChatClient::feeds()->request(channel, LS("query"), LS("history"), data);
}


/*!
 * Отправка пакета с запросом на получение офлайн сообщений.
 */
bool History::getOffline()
{
  if (ChatClient::state() != ChatClient::Online)
    return false;

  QVariantMap data;
  data.insert(LS("action"), LS("offline"));
  return ChatClient::feeds()->request(ChatClient::id(), LS("query"), LS("history"), data);
}


QList<MessageId> History::getLocal(const QList<MessageId> &ids)
{
  QList<MessageId> out;
  for (int i = 0; i < ids.size(); ++i) {
    MessageRecord record = HistoryDB::get(ids.at(i));
    if (!record.id) {
      out.append(ids.at(i));
      continue;
    }

    ChatClient::messages()->insert(new MessageNotice(record, /*parse=*/true));
  }

  return out;
}


void History::getLast()
{
  m_chatView->getLast(SimpleID::UserId);
}


void History::notify(const Notify &notify)
{
  if (notify.type() == Notify::FeedReply) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (n.match(LS("history"), LS("last"))) {
      lastReady(n);
    }
  }
  else if (notify.type() == Notify::ClearCache) {
    HistoryDB::clear();
  }

  if (notify.type() == Notify::FeedReply || notify.type() == Notify::QueryError) {
    if (!static_cast<const FeedNotify &>(notify).match(ChatClient::id(), LS("history"), LS("offline")))
      return;

    QTimer::singleShot(0, this, SLOT(getLast()));
  }
}


void History::open()
{
  QByteArray id = ChatClient::serverId();
  if (!id.isEmpty())
    HistoryDB::open(id, ChatCore::networks()->root(SimpleID::encode(id)));
}


/*!
 * Обработка ответа на успешный запрос \b last к фиду \b history.
 */
void History::lastReady(const FeedNotify &notify)
{
  QList<MessageId> ids = MessageId::toList(notify.json().value(LS("ids")).toString());
  if (ids.isEmpty())
    return;

  get(ids);
}


ChatPlugin *HistoryPlugin::create()
{
  m_plugin = new History(this);
  return m_plugin;
}

Q_EXPORT_PLUGIN2(History, HistoryPlugin);
