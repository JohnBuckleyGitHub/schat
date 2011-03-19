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

#include <QTextBrowser>
#include <QVBoxLayout>

#include "ui/tabs/ChatView.h"

class ChatViewPrivate
{
public:
  ChatViewPrivate(ChatView *parent)
    : q(parent)
  {
    browser = new QTextBrowser(q);
    browser->setFrameShape(QFrame::NoFrame);
    browser->document()->setDocumentMargin(2);

    mainLay = new QVBoxLayout(q);
    mainLay->addWidget(browser);
    mainLay->setMargin(0);
    mainLay->setSpacing(0);
  }

  ChatView *q;
  QTextBrowser *browser;
  QVBoxLayout *mainLay;
};


ChatView::ChatView(QWidget *parent)
  : QWidget(parent)
  , d(new ChatViewPrivate(this))
{
}


QVBoxLayout* ChatView::layout()
{
  return d->mainLay;
}


void ChatView::appendRawText(const QString &text)
{
  d->browser->append(text);
}
