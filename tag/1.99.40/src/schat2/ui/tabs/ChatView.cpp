/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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
#include <QTimer>
#include <QWebFrame>

#include "ChatCore.h"
#include "ChatNotify.h"
#include "ChatSettings.h"
#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "DateTime.h"
#include "debugstream.h"
#include "hooks/ChannelMenu.h"
#include "hooks/ChatViewHooks.h"
#include "JSON.h"
#include "messages/Message.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/tabs/ChatView.h"
#include "WebBridge.h"

ChatView::ChatView(const QByteArray &id, const QString &url, QWidget *parent)
  : QWebView(parent)
  , m_loaded(false)
  , m_id(id)
  , m_lastMessage(0)
{
  setAcceptDrops(false);
  page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

  setUrl(QUrl(url));
  connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(populateJavaScriptWindowObject()));

  setFocusPolicy(Qt::NoFocus);

  connect(ChatCore::settings(), SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
  connect(this, SIGNAL(linkClicked(const QUrl &)), SLOT(openUrl(const QUrl &)));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));

  createActions();
  retranslateUi();

  QTimer::singleShot(0, this, SLOT(start()));
}


ChatView::~ChatView()
{
  ChatViewHooks::remove(this);
}


void ChatView::add(const Message &msg)
{
  if (!msg.isValid())
    return;

  QVariantMap data = msg.data();
  data[LS("Hint")] = addHint(msg);

  if (!m_loaded)
    m_pendingMessages.enqueue(data);
  else
    emit message(data);
}


/*!
 * Добавление нового CSS файла.
 */
void ChatView::addCSS(const QString &file)
{
  evaluateJavaScript(LS("Loader.loadCSS(\"") + file + LS("\");"));
}


void ChatView::copy()
{
  triggerPageAction(QWebPage::Copy);
}


void ChatView::evaluateJavaScript(const QString &func, const QVariant &param)
{
  evaluateJavaScript(func + LS("(") + param.toString() + LS(");"));
}


void ChatView::evaluateJavaScript(const QString &js)
{
  if (m_loaded) {
    page()->mainFrame()->evaluateJavaScript(js);
  }
  else
    m_pendingJs.enqueue(js);
}


void ChatView::setId(const QByteArray &id)
{
  m_id = id;
  evaluateJavaScript(LS("Settings.id = \"") + SimpleID::encode(m_id) + LS("\";"));
}


QString ChatView::getId() const
{
  return SimpleID::encode(id());
}


/*!
 * Завершение загрузки документа.
 */
void ChatView::loadFinished()
{
  m_loaded = true;
  m_seconds->setChecked(SCHAT_OPTION("Display/Seconds").toBool());
  m_service->setChecked(SCHAT_OPTION("Display/Service").toBool());

  evaluateJavaScript("showSeconds", m_seconds->isChecked());
  evaluateJavaScript("showService", m_service->isChecked());
  setId(m_id);

  ChatViewHooks::loadFinished(this);

  while (!m_pendingJs.isEmpty())
    page()->mainFrame()->evaluateJavaScript(m_pendingJs.dequeue());

  while (!m_pendingMessages.isEmpty())
    emit message(m_pendingMessages.dequeue());

  while (!m_pendingFeeds.isEmpty())
    emit feed(m_pendingFeeds.dequeue());

  QTimer::singleShot(0, this, SLOT(alignChat()));
}


void ChatView::removeDay(const QString &day)
{
  m_messages.remove(day);
}


void ChatView::setLastMessage(qint64 date)
{
  if (date > m_lastMessage)
    m_lastMessage = date;
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
  if (!url.isEmpty() && url.scheme() != LS("chat"))
    menu.addAction(m_copyLink);

  menu.addSeparator();

  if (url.scheme() == "chat" && url.host() == LS("channel"))
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

  ClientChannel channel = ChatClient::channels()->get(id());
  if (channel && channel->data().value(LS("page")) == 1)
    menu.addAction(m_reload);
  else
    menu.addAction(m_clear);

  menu.addAction(m_selectAll);

  connect(&menu, SIGNAL(triggered(QAction *)), SLOT(menuTriggered(QAction *)));

  menu.exec(event->globalPos());
}


void ChatView::showEvent(QShowEvent *event)
{
  evaluateJavaScript(LS("alignChat();"));
  QWebView::showEvent(event);
}


void ChatView::alignChat()
{
  page()->mainFrame()->evaluateJavaScript(LS("Settings.scroll = true; alignChat();"));
}


void ChatView::menuTriggered(QAction *action)
{
  if (action == m_clear) {
    ChatNotify::start(Notify::ClearChat, id());
  }
  else if (action == m_reload) {
    reloadPage();
  }
  else if (action == m_seconds) {
    ChatCore::settings()->setValue(LS("Display/Seconds"), action->isChecked());
  }
  else if (action == m_service) {
    ChatCore::settings()->setValue(LS("Display/Service"), action->isChecked());
  }
}


void ChatView::notify(const Notify &notify)
{
  int type = notify.type();

  if (type == Notify::ClearChat) {
    if (m_id == notify.data().toByteArray())
      clearPage();
  }
  else if (type == Notify::FeedData || type == Notify::FeedReply || type == Notify::QueryError) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (n.channel() != id() && n.channel() != ChatClient::id())
      return;

    QVariantMap json = WebBridge::feed(n);
    if (json.isEmpty())
      return;

    if (!m_loaded)
      m_pendingFeeds.enqueue(json);
    else
      emit feed(json);
  }
}


void ChatView::openUrl(const QUrl &url)
{
  ChatUrls::open(url);
}


void ChatView::populateJavaScriptWindowObject()
{
  page()->mainFrame()->addToJavaScriptWindowObject(LS("SimpleChat"), WebBridge::i());
  page()->mainFrame()->addToJavaScriptWindowObject(LS("ChatView"), this);

  ChatViewHooks::init(this);
}


void ChatView::settingsChanged(const QString &key, const QVariant &value)
{
  if (key == LS("Display/Seconds")) {
    m_seconds->setChecked(value.toBool());
    evaluateJavaScript(LS("showSeconds"), value);
  }
  else if (key == LS("Display/Service")) {
    m_service->setChecked(value.toBool());
    evaluateJavaScript(LS("showService"), value);
  }
}


void ChatView::start()
{
  ChatViewHooks::add(this);
}


/*!
 * Формирует подсказку размещения сообщения.
 */
QVariantMap ChatView::addHint(const Message &message)
{
  QVariantMap out;
  out[LS("Hint")] = LS("end");

  const QVariantMap &data = message.data();
  qint64 date = data.value(LS("Date")).toLongLong();
  if (!date) {
    out[LS("Day")] = QDateTime::currentDateTime().toString(LS("yyyy_MM_dd"));
    return out;
  }

  const QString day = DateTime::toDateTime(date).toString(LS("yyyy_MM_dd"));
  out[LS("Day")] = day;

  if (data.value(LS("Status")) == LS("undelivered"))
    return out;

  QByteArray id = data.value(LS("Id")).toByteArray();
  if (id.isEmpty())
    return out;

  QMap<qint64, QByteArray> &messages = m_messages[day];
  messages[date] = id;
  if (messages.size() == 1)
    return out;

  QList<qint64> dates = messages.keys();
  int index = dates.indexOf(date);
  if (index == dates.size() - 1)
    return out;

  out[LS("Hint")] = LS("before");
  out[LS("Id")]   = QString(messages.value(dates.at(index + 1)));

  return out;
}


void ChatView::clearPage()
{
  m_messages.clear();
  emit reload();
}


void ChatView::createActions()
{
  m_copy = pageAction(QWebPage::Copy);
  m_copy->setIcon(SCHAT_ICON(EditCopy));

  m_copyLink = pageAction(QWebPage::CopyLinkToClipboard);
  m_copyLink->setIcon(SCHAT_ICON(EditCopy));

  m_clear = new QAction(SCHAT_ICON(EditClear), tr("Clear"), this);
  m_reload = new QAction(SCHAT_ICON(Reload), tr("Reload"), this);

  m_selectAll = pageAction(QWebPage::SelectAll);
  m_selectAll->setIcon(SCHAT_ICON(EditSelectAll));

  m_seconds = new QAction(tr("Seconds"), this);
  m_seconds->setCheckable(true);

  m_service = new QAction(tr("Service messages"), this);
  m_service->setCheckable(true);
}


void ChatView::reloadPage()
{
  emit reload();
}


void ChatView::retranslateUi()
{
  m_copy->setText(tr("Copy"));
  m_copyLink->setText(tr("Copy Link"));
  m_clear->setText(tr("Clear"));
  m_reload->setText(tr("Reload"));
  m_selectAll->setText(tr("Select All"));
}