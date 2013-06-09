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

//#define DEBUG_PERFORMANCE

#include <QAction>
#include <QContextMenuEvent>
#include <QEvent>
#include <QMenu>
#include <QTimer>
#include <QWebFrame>

#if defined(DEBUG_PERFORMANCE)
# include <QDebug>
#endif

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
  : WebView(parent)
  , m_loaded(false)
  , m_id(id)
  , m_lastMessage(0)
{
  setPage(new WebPage(this));

  page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

  setUrl(QUrl(url));
  connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(populateJavaScriptWindowObject()));

  setFocusPolicy(Qt::NoFocus);

  connect(ChatCore::settings(), SIGNAL(changed(const QString &, const QVariant &)), SLOT(settingsChanged(const QString &, const QVariant &)));
  connect(this, SIGNAL(linkClicked(const QUrl &)), SLOT(openUrl(const QUrl &)));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));

  setAcceptDrops(true);
  setIcons();
  createActions();
  retranslateUi();

  QTimer::singleShot(0, this, SLOT(start()));
}


ChatView::~ChatView()
{
  ChatViewHooks::remove(this);
}


bool ChatView::find(const QString &text, bool forward)
{
  QWebPage::FindFlags options = QWebPage::FindWrapsAroundDocument;
  if (!forward)
    options |= QWebPage::FindBackward;

  bool found = findText(text, options);

  options = QWebPage::HighlightAllOccurrences;
  findText(QString(), options);
  findText(text, options);

  if (!found && text.isEmpty())
    found = true;

  return found;
}


void ChatView::add(const Message &msg)
{
  if (!msg.isValid())
    return;

  QVariantMap data = msg.data();
  data[LS("Hint")] = addHint(msg);

  m_messagesQueue.append(data);
  if (m_loaded && m_messagesQueue.size() == 1)
    QTimer::singleShot(0, this, SLOT(startTasks()));
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


void ChatView::allowFocus(bool allow)
{
  if (allow) {
    setFocusPolicy(Qt::WheelFocus);
    setFocus();
  }
  else {
    setFocusPolicy(Qt::NoFocus);
    ChatNotify::start(Notify::SetSendFocus);
  }
}


/*!
 * Завершение загрузки документа.
 */
void ChatView::loadFinished()
{
  m_loaded = true;
  m_autoscroll->setChecked(ChatCore::settings()->value(SETTINGS_AUTO_SCROLL).toBool());
  m_seconds->setChecked(ChatCore::settings()->value(SETTINGS_DISPLAY_SECONDS).toBool());
  m_service->setChecked(ChatCore::settings()->value(SETTINGS_DISPLAY_SERVICE).toBool());

  evaluateJavaScript("showSeconds", m_seconds->isChecked());
  evaluateJavaScript("showService", m_service->isChecked());
  setId(m_id);

  ChatViewHooks::loadFinished(this);

  while (!m_pendingJs.isEmpty())
    page()->mainFrame()->evaluateJavaScript(m_pendingJs.dequeue());

  QTimer::singleShot(0, this, SLOT(startTasks()));

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


void ChatView::contextMenu(QMenu *menu, const QWebHitTestResult &result)
{
  menu->addSeparator();

  const QUrl url = result.linkUrl();
  if (url.scheme() == LS("chat") && url.host() == LS("channel"))
    Hooks::ChannelMenu::bind(menu, ChatUrls::channel(url), Hooks::ChatViewScope);
  else
    Hooks::ChannelMenu::bind(menu, ChatClient::channels()->get(m_id), Hooks::ChatViewScope);

  menu->addSeparator();
  QMenu *display = menu->addMenu(SCHAT_ICON(Gear), tr("Display"));
  display->addAction(m_seconds);
  display->addAction(m_service);
  developerMenu(display);
  display->removeAction(pageAction(QWebPage::Reload));

  menu->addAction(m_autoscroll);
  menu->addSeparator();

  ClientChannel channel = ChatClient::channels()->get(id());
  if (channel && channel->data().value(LS("page")) == 1)
    menu->addAction(m_reload);
  else
    menu->addAction(m_clear);

  if (!result.isContentEditable()) {
    menu->removeAction(pageAction(QWebPage::SelectAll));
    menu->addAction(pageAction(QWebPage::SelectAll));
  }

  menu->removeAction(pageAction(QWebPage::Reload));
  menu->removeAction(pageAction(QWebPage::InspectElement));

  connect(menu, SIGNAL(triggered(QAction*)), SLOT(menuTriggered(QAction*)));
}


void ChatView::dragEnterEvent(QDragEnterEvent *event)
{
  ChatViewHooks::dragEnterEvent(this, event);
}


void ChatView::dropEvent(QDropEvent *event)
{
  ChatViewHooks::dropEvent(this, event);
}


void ChatView::retranslateUi()
{
  WebView::retranslateUi();

  m_autoscroll->setText(tr("Autoscroll"));
  m_clear->setText(tr("Clear"));
  m_reload->setText(tr("Reload"));
  m_seconds->setText(tr("Seconds"));
  m_service->setText(tr("Service messages"));
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
  if (action == m_clear)
    ChatNotify::start(Notify::ClearChat, id());
  else if (action == m_reload)
    reloadPage();
  else if (action == m_seconds)
    ChatCore::settings()->setValue(SETTINGS_DISPLAY_SECONDS, action->isChecked());
  else if (action == m_service)
    ChatCore::settings()->setValue(SETTINGS_DISPLAY_SERVICE, action->isChecked());
  else if (action == m_autoscroll)
    ChatCore::settings()->setValue(SETTINGS_AUTO_SCROLL, action->isChecked());
}


void ChatView::notify(const Notify &notify)
{
  const int type = notify.type();

  if (type == Notify::ClearChat) {
    if (m_id == notify.data().toByteArray())
      clearPage();
  }
  else if (type == Notify::FeedData || type == Notify::FeedReply) {
    const FeedNotify &n = static_cast<const FeedNotify &>(notify);
    if (n.channel() == id() || n.channel() == ChatClient::id() || (n.channel() == ChatClient::serverId() && n.feed() == FEED_NAME_ACL)) {
      const QVariantMap json = WebBridge::feed(n);
      if (json.isEmpty())
        return;

      if (!m_loaded)
        m_pendingFeeds.enqueue(json);
      else
        emit feed(json);
    }
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
  if (key == SETTINGS_DISPLAY_SECONDS) {
    m_seconds->setChecked(value.toBool());
    evaluateJavaScript(LS("showSeconds"), value);
  }
  else if (key == SETTINGS_DISPLAY_SERVICE) {
    m_service->setChecked(value.toBool());
    evaluateJavaScript(LS("showService"), value);
  }
  else if (key == SETTINGS_AUTO_SCROLL)
    m_autoscroll->setChecked(value.toBool());
}


void ChatView::start()
{
  ChatViewHooks::add(this);
}


void ChatView::startTasks()
{
# if defined(DEBUG_PERFORMANCE)
  QTime t;
  if (!m_messagesQueue.isEmpty())
    qDebug() << "ChatView::startTasks()" << SimpleID::encode(m_id);
# endif

  if (!m_messagesQueue.isEmpty()) {
#   if defined(DEBUG_PERFORMANCE)
    t.start();
#   endif

    emit messages(m_messagesQueue);

#   if defined(DEBUG_PERFORMANCE)
    const int elapsed = t.elapsed();
    qDebug() << "ChatView::m_messagesQueue" << m_messagesQueue.size() << "items" << elapsed << "ms";
#   endif

    m_messagesQueue.clear();
  }
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
  m_autoscroll = new QAction(tr("Autoscroll"), this);
  m_autoscroll->setCheckable(true);

  m_clear = new QAction(SCHAT_ICON(EditClear), tr("Clear"), this);
  m_reload = new QAction(SCHAT_ICON(Reload), tr("Reload"), this);

  m_seconds = new QAction(tr("Seconds"), this);
  m_seconds->setCheckable(true);

  m_service = new QAction(tr("Service messages"), this);
  m_service->setCheckable(true);
}


void ChatView::reloadPage()
{
  emit reload();
}
