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

#include "net/SimpleID.h"
#include "SendFileChatView.h"
#include "SendFilePlugin_p.h"
#include "sglobal.h"
#include "ui/tabs/ChatView.h"

SendFileChatView::SendFileChatView(SendFilePluginImpl *plugin)
  : ChatViewHooks(plugin)
  , m_plugin(plugin)
{
}

void SendFileChatView::initImpl(ChatView *view)
{
  if (SimpleID::typeOf(view->id()) != SimpleID::UserId)
    return;

  view->addJS(LS("qrc:/js/SendFile/SendFile.js"));
  view->page()->mainFrame()->addToJavaScriptWindowObject(LS("SendFile"), m_plugin);
}


void SendFileChatView::loadFinishedImpl(ChatView *view)
{
  if (SimpleID::typeOf(view->id()) == SimpleID::UserId)
    view->addCSS(LS("qrc:/css/SendFile/SendFile.css"));
}
