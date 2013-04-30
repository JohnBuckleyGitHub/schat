/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include <QWebFrame>

#include "ChatNotify.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientFeeds.h"
#include "client/SimpleClient.h"
#include "HistoryChatView.h"
#include "HistoryDB.h"
#include "HistoryPlugin_p.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "ui/tabs/ChatView.h"

HistoryChatView::HistoryChatView(QObject *parent)
  : ChatViewHooks(parent)
{
  connect(ChatClient::io(), SIGNAL(ready()), SLOT(ready()));
  connect(ChatNotify::i(), SIGNAL(notify(Notify)), SLOT(notify(Notify)));
}


void HistoryChatView::addImpl(ChatView *view)
{
  if (compatible(view->id()) && sync(view->id()))
    emit loading(SimpleID::encode(view->id()));
}


void HistoryChatView::initImpl(ChatView *view)
{
  view->addJS(LS("qrc:/js/History/days.js"));

  if (compatible(view->id())) {
    view->page()->mainFrame()->addToJavaScriptWindowObject(LS("HistoryView"), this);
    view->addJS(LS("qrc:/js/History/History.js"));
  }
}


void HistoryChatView::loadFinishedImpl(ChatView *view)
{
  view->addCSS(LS("qrc:/css/History/History.css"));
}


void HistoryChatView::notify(const Notify &notify)
{
  if (notify.type() == Notify::FeedReply) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (n.feed() == FEED_NAME_MESSAGES && n.path() == LS("last")) {
      if (n.status() == Notice::NotModified) {
        const QList<QByteArray> messages = HistoryDB::last(n.channel());
        HistoryImpl::get(n.channel(), messages);
        emulateLast(n.channel(), messages);
      }
      else if (n.status() == Notice::OK && !n.json().contains(LS("before")) && !n.json().contains(LS("emulated")))
        HistoryDB::add(n.channel(), n.json().value(LS("messages")).toStringList());
    }
  }
}


/*!
 * Запрос последних сообщений для всех открытых каналов.
 */
void HistoryChatView::ready()
{
  ChatClientLocker locker;

  foreach (ChatView *view, i()->views()) {
    const QByteArray &id = view->id();
    if (compatible(id) && sync(id, view->lastMessage())) {
      emit loading(SimpleID::encode(id));
    }
  }

  ClientFeeds::request(ChatClient::id(), FEED_METHOD_GET, LS("messages/offline"));
}


/*!
 * Возвращает \b true если идентификатор является идентификатором обычного канала или пользователя.
 */
bool HistoryChatView::compatible(const QByteArray &id) const
{
  int type = SimpleID::typeOf(id);
  if (type == SimpleID::ChannelId || type == SimpleID::UserId)
    return true;

  return false;
}


/*!
 * Синхронизации списка последних сообщений в канале.
 *
 * \param id   Идентификатор канала.
 * \param date Дата последнего полученного сообщения, если равно 0 запрашиваются 20 последних сообщений.
 */
bool HistoryChatView::sync(const QByteArray &id, qint64 date)
{
  if (ChatClient::state() != ChatClient::Online) {
    HistoryImpl::getLocal(HistoryDB::last(id));
    return false;
  }

  if (date)
    return HistoryImpl::since(id, date);

  QVariantMap json;
  const QString tag = HistoryDB::tag(id);
  if (!tag.isEmpty())
    json[LS("tag")] = tag;

  return ClientFeeds::request(id, FEED_METHOD_GET, LS("messages/last"), json);
}


/*!
 * Эмуляция ответа на запрос последних сообщений.
 *
 * \param channelId Идентификатор канала.
 * \param ids       Список идентификаторов сообщений.
 */
void HistoryChatView::emulateLast(const QByteArray &channelId, const QList<QByteArray> &ids)
{
  QVariantMap data;
  data[LS("count")]    = ids.size();
  data[LS("messages")] = MessageNotice::encode(ids);
  data[LS("emulated")] = true;

  FeedNotify *notify = new FeedNotify(Notify::FeedReply, channelId, LS("messages/last"), data);
  ChatNotify::start(notify);
}