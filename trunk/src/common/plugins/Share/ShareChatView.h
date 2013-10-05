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

#ifndef SHARECHATVIEW_H_
#define SHARECHATVIEW_H_

#include <QUrl>
#include <QStringList>

#include "hooks/ChatViewHooks.h"

class Share;

class ShareChatView : public ChatViewHooks
{
  Q_OBJECT

public:
  ShareChatView(Share *share);

protected:
  bool onDragEnterEvent(ChatView *view, QDragEnterEvent *event);
  bool onDropEvent(ChatView *view, QDropEvent *event);

private:
  QStringList getFiles(const QList<QUrl> &urls) const;

  Share *m_share;
};

#endif /* SHARECHATVIEW_H_ */