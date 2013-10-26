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

#include <QDragEnterEvent>
#include <QNetworkDiskCache>
#include <QWebFrame>

#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "Path.h"
#include "sglobal.h"
#include "ShareChatView.h"
#include "ShareDnD.h"
#include "SharePlugin_p.h"
#include "ui/tabs/ChatView.h"


ShareChatView::ShareChatView(Share *share)
  : ChatViewHooks(share)
  , m_share(share)
{
}


bool ShareChatView::onDragEnterEvent(ChatView *view, QDragEnterEvent *event)
{
  m_id.init(view->id());
  if (!event->mimeData()->hasUrls() || !isAcceptable())
    return false;

  if (ShareDnD::getFiles(event->mimeData()).isEmpty() && ShareDnD::getUrls(event->mimeData()).isEmpty())
    return false;

  event->acceptProposedAction();
  return true;
}


bool ShareChatView::onDropEvent(ChatView *view, QDropEvent *event)
{
  m_id.init(view->id());
  if (!event->mimeData()->hasUrls() || !isAcceptable())
    return false;

  event->acceptProposedAction();

  if (!m_share->upload(m_id, ShareDnD::getFiles(event->mimeData())))
    return m_share->upload(m_id, ShareDnD::getUrls(event->mimeData()));

  return true;
}


void ShareChatView::initImpl(ChatView *view)
{
  m_id.init(view->id());
  if (m_id.type() != ChatId::ChannelId)
    return;

  QNetworkDiskCache *cache = new QNetworkDiskCache(this);
  cache->setCacheDirectory(Path::cache() + LS("/cache"));
  view->page()->networkAccessManager()->setCache(cache);

  view->page()->mainFrame()->addToJavaScriptWindowObject(LS("Share"), m_share);
  view->addJS(LS("qrc:/js/Share/share.js"));
}


void ShareChatView::loadFinishedImpl(ChatView *view)
{
  m_id.init(view->id());
  if (m_id.type() != ChatId::ChannelId)
    return;

  view->addCSS(LS("qrc:/css/Share/share.css"));
}


bool ShareChatView::isAcceptable() const
{
  if (m_id.type() != ChatId::ChannelId)
    return false;

  ClientChannel channel = ChatClient::channels()->get(m_id.toByteArray());
  if (!channel)
    return false;

  return channel->feed("info")->data().value("images", true) != false;
}
