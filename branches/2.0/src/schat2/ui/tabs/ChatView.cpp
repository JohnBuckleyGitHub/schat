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

#include <QDesktopServices>
#include <QWebFrame>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "debugstream.h"
#include "ui/tabs/ChatView.h"

ChatView::ChatView(QWidget *parent)
  : QWebView(parent)
  , m_loaded(false)
{
  setAcceptDrops(false);
  page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

  setUrl(QUrl("qrc:/html/ChatView.html"));
  connect(this, SIGNAL(loadFinished(bool)), SLOT(loadFinished()));
  connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(populateJavaScriptWindowObject()));

  QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

  setFocusPolicy(Qt::NoFocus);

  connect(ChatCore::i()->settings(), SIGNAL(changed(const QList<int> &)), SLOT(settingsChanged(const QList<int> &)));
  connect(this, SIGNAL(linkClicked(const QUrl &)), SLOT(openUrl(const QUrl &)));
}


/*!
 * Базовая функция добавления сообщения.
 * \deprecated ChatView::appendRawMessage().
 */
void ChatView::appendRawMessage(const QString &message)
{
  QString js = message;
  js.replace("\"","\\\"");
  js.replace("\n","\\n");
  js = QString("appendMessage(\"%1\");").arg(js);

  if (m_loaded) {
    page()->mainFrame()->evaluateJavaScript(js);
  }
  else
    m_pendingJs.enqueue(js);
}


void ChatView::evaluateJavaScript(const QString &js)
{
  if (m_loaded) {
    page()->mainFrame()->evaluateJavaScript(js);
  }
  else
    m_pendingJs.enqueue(js);
}


/*!
 * Завершение загрузки документа.
 */
void ChatView::loadFinished()
{
  m_loaded = true;
  showSeconds(SCHAT_OPTION(ShowSeconds).toBool());

  while (!m_pendingJs.isEmpty())
    page()->mainFrame()->evaluateJavaScript(m_pendingJs.dequeue());
}


void ChatView::openUrl(const QUrl &url)
{
  QDesktopServices::openUrl(url);
}


void ChatView::populateJavaScriptWindowObject()
{
  page()->mainFrame()->addToJavaScriptWindowObject("SimpleChat", ChatCore::i());
}


void ChatView::settingsChanged(const QList<int> &keys)
{
  if (keys.contains(ChatSettings::ShowSeconds)) {
    showSeconds(SCHAT_OPTION(ShowSeconds).toBool());
  }
}


/*!
 * Отображает/скрывает секунды в окне чата.
 */
void ChatView::showSeconds(bool show)
{
  if (show)
    evaluateJavaScript("showSeconds(true)");
  else
    evaluateJavaScript("showSeconds(false)");
}
