/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#ifndef CHATVIEWACTION_H_
#define CHATVIEWACTION_H_

#include <QUrl>

class ChatView;

class ChatViewAction
{
public:
  ChatViewAction(ChatView *view = 0);
  virtual ~ChatViewAction() {}
  virtual bool exec(const QString & /*id*/, const QString & /*button*/) { return true; }

protected:
  ChatView *m_view;
};


class UrlAction : public ChatViewAction
{
public:
  UrlAction(const QUrl &url, ChatView *view = 0);
  bool exec(const QString &id, const QString &button);

private:
  QUrl m_url;
};

#endif /* CHATVIEWACTION_H_ */
