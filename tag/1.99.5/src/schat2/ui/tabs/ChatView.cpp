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

#include <QAction>
#include <QContextMenuEvent>
#include <QEvent>
#include <QMenu>
#include <QWebFrame>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "ChatUrls.h"
#include "debugstream.h"
#include "hooks/ChannelMenu.h"
#include "hooks/ChatViewHooks.h"
#include "JSON.h"
#include "messages/Message.h"
#include "net/SimpleID.h"
#include "ui/ChatIcons.h"
#include "ui/tabs/ChatView.h"

ChatView::ChatView(const QByteArray &id, const QString &url, QWidget *parent)
  : QWebView(parent)
  , m_loaded(false)
  , m_id(id)
{
  setAcceptDrops(false);
  page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

  setUrl(QUrl(url));
  connect(this, SIGNAL(loadFinished(bool)), SLOT(loadFinished()));
  connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(populateJavaScriptWindowObject()));

  setFocusPolicy(Qt::NoFocus);

  connect(ChatCore::i()->settings(), SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
  connect(this, SIGNAL(linkClicked(const QUrl &)), SLOT(openUrl(const QUrl &)));

  createActions();
  retranslateUi();
}


void ChatView::add(const Message &message)
{
  if (!message.isValid())
    return;

  evaluateJavaScript(message.func() + "(" + JSON::quote(message.json()) + ");");
}


void ChatView::copy()
{
  triggerPageAction(QWebPage::Copy);
}


void ChatView::evaluateJavaScript(const QString &func, const QVariant &param)
{
  evaluateJavaScript(func + "(" + param.toString() + ");");
}


void ChatView::evaluateJavaScript(const QString &js)
{
  if (m_loaded) {
    page()->mainFrame()->evaluateJavaScript(js);
  }
  else
    m_pendingJs.enqueue(js);
}


void ChatView::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWebView::changeEvent(event);
}


/*!
 * Показ контекстного меню.
 */
void ChatView::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu(this);

  if (!selectedText().isEmpty())
    menu.addAction(m_copy);

  QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());
  QUrl url = r.linkUrl();
  if (!url.isEmpty() && url.scheme() != "chat")
    menu.addAction(m_copyLink);

  menu.addSeparator();

  if (url.scheme() == "chat" && url.host() == "channel")
    Hooks::ChannelMenu::bind(&menu, url);
  else
    Hooks::ChannelMenu::bind(&menu, m_id);

  QMenu display(tr("Display"), this);
  display.setIcon(SCHAT_ICON(Gear));
  menu.addSeparator();
  menu.addMenu(&display);
  display.addAction(m_seconds);
  display.addAction(m_service);

  if (QWebSettings::globalSettings()->testAttribute(QWebSettings::DeveloperExtrasEnabled)) {
    display.addSeparator();
    display.addAction(pageAction(QWebPage::InspectElement));
  }

  menu.addSeparator();
  menu.addAction(m_clear);
  menu.addAction(m_selectAll);

  connect(&menu, SIGNAL(triggered(QAction *)), SLOT(menuTriggered(QAction *)));

  menu.exec(event->globalPos());
}


void ChatView::showEvent(QShowEvent *event)
{
  evaluateJavaScript("alignChat();");
  QWebView::showEvent(event);
}


/*!
 * Завершение загрузки документа.
 */
void ChatView::loadFinished()
{
  m_loaded = true;
  m_seconds->setChecked(SCHAT_OPTION("ShowSeconds").toBool());
  m_service->setChecked(SCHAT_OPTION("ShowServiceMessages").toBool());

  evaluateJavaScript("showSeconds", m_seconds->isChecked());
  evaluateJavaScript("showService", m_service->isChecked());

  while (!m_pendingJs.isEmpty())
    page()->mainFrame()->evaluateJavaScript(m_pendingJs.dequeue());

  ChatViewHooks::loadFinished(this);
}


void ChatView::menuTriggered(QAction *action)
{
  if (action == m_clear) {
    m_loaded = false;
    page()->triggerAction(QWebPage::ReloadAndBypassCache);
    emit reloaded();
  }
  else if (action == m_seconds) {
    ChatCore::settings()->setValue(QLatin1String("ShowSeconds"), action->isChecked());
  }
  else if (action == m_service) {
    ChatCore::settings()->setValue(QLatin1String("ShowServiceMessages"), action->isChecked());
  }
}


void ChatView::openUrl(const QUrl &url)
{
  ChatUrls::open(url);
}


void ChatView::populateJavaScriptWindowObject()
{
  page()->mainFrame()->addToJavaScriptWindowObject("SimpleChat", ChatCore::i());
}


void ChatView::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == QLatin1String("ShowSeconds")) {
    m_seconds->setChecked(value.toBool());
    evaluateJavaScript("showSeconds", value);
  }
  else if (key == QLatin1String("ShowServiceMessages")) {
    m_service->setChecked(value.toBool());
    evaluateJavaScript("showService", value);
  }
}


void ChatView::createActions()
{
  m_copy = pageAction(QWebPage::Copy);
  m_copy->setIcon(SCHAT_ICON(EditCopy));

  m_copyLink = pageAction(QWebPage::CopyLinkToClipboard);
  m_copyLink->setIcon(SCHAT_ICON(EditCopy));

  m_clear = new QAction(SCHAT_ICON(EditClear), tr("Clear"), this);

  m_selectAll = pageAction(QWebPage::SelectAll);
  m_selectAll->setIcon(SCHAT_ICON(EditSelectAll));

  m_seconds = new QAction(tr("Seconds"), this);
  m_seconds->setCheckable(true);

  m_service = new QAction(tr("Service messages"), this);
  m_service->setCheckable(true);
}


void ChatView::retranslateUi()
{
  m_copy->setText(tr("Copy"));
  m_copyLink->setText(tr("Copy Link"));
  m_clear->setText(tr("Clear"));
  m_selectAll->setText(tr("Select All"));
}