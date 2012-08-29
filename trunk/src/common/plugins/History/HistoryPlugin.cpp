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

HistoryImpl::HistoryImpl(QObject *parent)
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
bool HistoryImpl::get(const QByteArray &id, const QList<QByteArray> &ids)
{
  if (ids.isEmpty())
    return false;

  QList<QByteArray> required = getLocal(ids);
  if (required.isEmpty() || ChatClient::state() != ChatClient::Online)
    return false;

  QVariantMap data;
  data[LS("messages")] = MessageNotice::encode(required);
  return ClientFeeds::request(id, LS("get"), LS("messages/fetch"), data);
}


/*!
 * Отправка пакета с запросом на получение последних сообщений в канале.
 *
 * \param id Идентификатор канала.
 */
bool HistoryImpl::getLast(const QByteArray &id)
{
  if (ChatClient::state() != ChatClient::Online) {
    getLocal(HistoryDB::last(id, 20));
    return false;
  }

  return ClientFeeds::request(id, LS("get"), LS("messages/last"));
}


/*!
 * Отправка пакета с запросом на получение офлайн сообщений.
 *
 * \deprecated Необходимо использовать GET запрос.
 */
bool HistoryImpl::getOffline()
{
  if (ChatClient::state() != ChatClient::Online)
    return false;

  QVariantMap data;
  data.insert(LS("action"), LS("offline"));
  return ClientFeeds::request(ChatClient::id(), LS("query"), LS("history"), data);
}


QList<QByteArray> HistoryImpl::getLocal(const QList<QByteArray> &ids)
{
  QList<QByteArray> out;
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


void HistoryImpl::getLast()
{
  m_chatView->getLast(SimpleID::UserId);
}


void HistoryImpl::notify(const Notify &notify)
{
  if (notify.type() == Notify::FeedReply) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (n.name() == LS("messages/last"))
      get(n.channel(), MessageNotice::decode(n.json().value(LS("messages")).toStringList()));
  }
  else if (notify.type() == Notify::ClearCache) {
    HistoryDB::clear();
  }
}


void HistoryImpl::open()
{
  QByteArray id = ChatClient::serverId();
  if (!id.isEmpty())
    HistoryDB::open(id, ChatCore::networks()->root(SimpleID::encode(id)));
}


ChatPlugin *HistoryPlugin::create()
{
  m_plugin = new HistoryImpl(this);
  return m_plugin;
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(History, HistoryPlugin);
#endif
