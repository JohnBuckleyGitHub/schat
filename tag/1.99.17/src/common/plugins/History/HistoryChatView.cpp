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

#include "client/ChatClient.h"
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


void HistoryChatView::getLast(int type) const
{
  ChatClient::io()->lock();

  foreach (ChatView *view, i()->views()) {
    if (SimpleID::typeOf(view->id()) == type)
      History::getLast(view->id());
  }

  ChatClient::io()->unlock();
}


void HistoryChatView::addImpl(ChatView *view)
{
  if (SimpleID::typeOf(view->id()) == SimpleID::ChannelId || SimpleID::typeOf(view->id()) == SimpleID::UserId)
    History::getLast(view->id());
}


void HistoryChatView::ready()
{
  getLast(SimpleID::ChannelId);
  History::getOffline();
}
