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

#include "Path.h"
#include "sglobal.h"
#include "ShareChatView.h"
#include "SharePlugin_p.h"
#include "ui/tabs/ChatView.h"
#include "ShareDnD.h"


ShareChatView::ShareChatView(Share *share)
  : ChatViewHooks(share)
  , m_share(share)
{
}


bool ShareChatView::onDragEnterEvent(ChatView *view, QDragEnterEvent *event)
{
  if (ChatId(view->id()).type() != ChatId::ChannelId || !event->mimeData()->hasUrls())
    return false;

  const QList<QUrl> urls = event->mimeData()->urls();
  if (ShareDnD::getFiles(urls).isEmpty() && ShareDnD::getUrls(urls).isEmpty())
    return false;

  event->acceptProposedAction();
  return true;
}


bool ShareChatView::onDropEvent(ChatView *view, QDropEvent *event)
{
  const ChatId id(view->id());
  if (ChatId(view->id()).type() != ChatId::ChannelId || !event->mimeData()->hasUrls())
    return false;

  event->acceptProposedAction();

  const QList<QUrl> urls = event->mimeData()->urls();
  if (!m_share->upload(id, ShareDnD::getFiles(urls)))
    return m_share->upload(id, ShareDnD::getUrls(urls));

  return true;
}


void ShareChatView::initImpl(ChatView *view)
{
  if (ChatId(view->id()).type() != ChatId::ChannelId)
    return;

  QNetworkDiskCache *cache = new QNetworkDiskCache(this);
  cache->setCacheDirectory(Path::cache() + LS("/cache"));
  view->page()->networkAccessManager()->setCache(cache);

  view->page()->mainFrame()->addToJavaScriptWindowObject(LS("Share"), m_share);
  view->addJS(LS("qrc:/js/Share/share.js"));
}


void ShareChatView::loadFinishedImpl(ChatView *view)
{
  if (ChatId(view->id()).type() != ChatId::ChannelId)
    return;

  view->addCSS(LS("qrc:/css/Share/share.css"));
}
