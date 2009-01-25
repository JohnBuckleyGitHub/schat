/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include <QtGui>
#include <QtWebKit>

#include "chatview.h"
#include "chatwindowstyle.h"
#include "chatwindowstyleoutput.h"

/*!
 * \brief Конструктор класса ChatView.
 */
ChatView::ChatView(QWidget *parent)
  : QWidget(parent)
{
  m_view = new QWebView(this);
  m_view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  m_style = new ChatWindowStyleOutput("Default", "");
  m_view->setHtml(m_style->makeSkeleton());

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(m_view);
  mainLay->setMargin(0);

  connect(m_view, SIGNAL(linkClicked(const QUrl &)), SLOT(linkClicked(const QUrl &)));
  setFocusPolicy(Qt::NoFocus);
}


ChatView::~ChatView()
{
  delete m_style;
}


void ChatView::addServiceMsg(const QString &msg)
{
  appendMessage(m_style->makeStatus(msg));
}


void ChatView::addMsg(const QString &sender, const QString &message, bool direction)
{
  bool same = false;

  if (m_prev.isEmpty() || m_prev != sender)
    m_prev = sender;
  else
    same = true;

  appendMessage(m_style->makeMessage(sender, message, direction, same), same);
}


void ChatView::linkClicked(const QUrl &url)
{
//  qDebug() << "ChatView::linkClicked()" << url.toString();

  QDesktopServices::openUrl(url);

}


void ChatView::appendMessage(QString message, bool same_from)
{
//  qDebug() << "ChatView::appendMessage()" << message << same_from;

  QString js_message = QString("append%2Message(\"%1\");").arg(message.replace("\"","\\\"").replace("\n","\\n")).arg(same_from?"Next":"");
  m_view->page()->mainFrame()->evaluateJavaScript(js_message);
}
