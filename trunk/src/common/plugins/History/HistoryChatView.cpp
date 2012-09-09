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

#include <QWebFrame>

#include "client/ChatClient.h"
#include "client/ClientFeeds.h"
#include "client/SimpleClient.h"
#include "HistoryChatView.h"
#include "HistoryPlugin_p.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "ui/tabs/ChatView.h"

HistoryChatView::HistoryChatView(QObject *parent)
  : ChatViewHooks(parent)
{
  connect(ChatClient::io(), SIGNAL(ready()), SLOT(ready()));
}


void HistoryChatView::addImpl(ChatView *view)
{
  if (compatible(view->id()) && HistoryImpl::last(view->id()))
    emit loading(SimpleID::encode(view->id()));
}


void HistoryChatView::initImpl(ChatView *view)
{
  if (compatible(view->id())) {
    view->page()->mainFrame()->addToJavaScriptWindowObject(LS("HistoryView"), this);
    view->addJS(LS("qrc:/js/History/History.js"));
  }
}


void HistoryChatView::loadFinishedImpl(ChatView *view)
{
  if (compatible(view->id()))
    view->addCSS(LS("qrc:/css/History/History.css"));
}


/*!
 * Запрос последних сообщений для всех открытых каналов.
 */
void HistoryChatView::ready()
{
  ChatClient::io()->lock();

  bool sent = false;
  foreach (ChatView *view, i()->views()) {
    const QByteArray &id = view->id();
    if (compatible(id)) {
      if (view->lastMessage()) {
        QVariantMap data;
        data[LS("date")] = view->lastMessage();
        sent = ClientFeeds::request(id, LS("get"), LS("messages/since"), data);
      }
      else
        sent = HistoryImpl::last(id);

      if (sent)
        emit loading(SimpleID::encode(id));
    }
  }

  ClientFeeds::request(ChatClient::id(), LS("get"), LS("messages/offline"));
  ChatClient::io()->unlock();
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
