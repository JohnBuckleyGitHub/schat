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
#include <QFileInfo>

#include "sglobal.h"
#include "ShareChatView.h"
#include "SharePlugin_p.h"
#include "ui/tabs/ChatView.h"

#define MAX_SIZE 10485760 /* 10 MB */

ShareChatView::ShareChatView(Share *share)
  : ChatViewHooks(share)
  , m_share(share)
{
}


bool ShareChatView::onDragEnterEvent(ChatView *view, QDragEnterEvent *event)
{
  if (ChatId(view->id()).type() != ChatId::ChannelId || !event->mimeData()->hasUrls() || getFiles(event->mimeData()->urls()).isEmpty())
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
  m_share->upload(id, getFiles(event->mimeData()->urls()));
  return true;
}


QStringList ShareChatView::getFiles(const QList<QUrl> &urls) const
{
  QStringList out;
  qint64 size = 0;

  foreach (const QUrl &url, urls) {
    if (url.scheme() == LS("file")) {
      const QFileInfo fi(url.toLocalFile());
      if (!fi.exists() || !fi.isFile())
        continue;

      size += fi.size();
      if (size > MAX_SIZE)
        continue;

      const QString suffix = fi.suffix().toLower();
      if (suffix == LS("png") || suffix == LS("gif") || suffix == LS("jpg") || suffix == LS("jpeg"))
        out.append(fi.absoluteFilePath());
    }
  }

  return out;
}
