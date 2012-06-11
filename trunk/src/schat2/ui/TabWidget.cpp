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
#include <QToolBar>
#include <QToolButton>

#include "debugstream.h"

#include "actions/MenuBuilder.h"
#include "ChatCore.h"
#include "ChatIcons.h"
#include "ChatNotify.h"
#include "ChatSettings.h"
#include "ChatUrls.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/SimpleClient.h"
#include "messages/ChannelMessage.h"
#include "net/SimpleID.h"
#include "NetworkManager.h"
#include "sglobal.h"
#include "ui/MainToolBar.h"
#include "ui/SoundButton.h"
#include "ui/TabBar.h"
#include "ui/tabs/AboutTab.h"
#include "ui/tabs/ChannelTab.h"
#include "ui/tabs/ChatView.h"
#include "ui/tabs/PrivateTab.h"
#include "ui/tabs/ProgressTab.h"
#include "ui/tabs/ServerTab.h"
#include "ui/tabs/SettingsTab.h"
#include "ui/tabs/UserView.h"
#include "ui/tabs/WelcomeTab.h"
#include "ui/TabsToolBar.h"
#include "ui/TabWidget.h"
#include "ui/TrayIcon.h"

TabWidget *TabWidget::m_self = 0;

TabWidget::TabWidget(QWidget *parent)
  : QTabWidget(parent)
  , m_tabBar(new TabBar(this))
{
  m_self = this;
  setTabBar(m_tabBar);

  #if defined(Q_WS_WIN)
  setDocumentMode(false);
  #else
  setDocumentMode(true);
  #endif

  #if !defined(Q_WS_MAC)
  setStyleSheet("QToolBar { margin:0px; border:0px; }" );
  #endif

  QWebSettings::globalSettings()->setFontSize(QWebSettings::DefaultFontSize, fontInfo().pixelSize());
  QWebSettings::globalSettings()->setFontSize(QWebSettings::DefaultFixedFontSize, fontInfo().pixelSize());
  QWebSettings::globalSettings()->setFontFamily(QWebSettings::StandardFont, fontInfo().family());
  QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, SCHAT_OPTION("Labs/DeveloperExtras").toBool());

  m_serverTab = new ServerTab(this);
  m_serverTab->setVisible(false);

  showWelcome();

  m_tray = new TrayIcon(this);
  QTimer::singleShot(0, m_tray, SLOT(show()));

  createToolBars();
  retranslateUi();

  connect(this, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
  connect(this, SIGNAL(currentChanged(int)), SLOT(currentChanged(int)));
  connect(ChatClient::channels(), SIGNAL(channel(const QByteArray &)), SLOT(addChannel(const QByteArray &)));
  connect(ChatClient::io(), SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int, int)));
  connect(m_serverTab, SIGNAL(actionTriggered(bool)), SLOT(openTab()));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
}


AbstractTab *TabWidget::widget(int index) const
{
  return static_cast<AbstractTab*>(QTabWidget::widget(index));
}


ClientChannel TabWidget::channel(const QByteArray &id) const
{
  if (m_channels.contains(id))
    return m_channels.value(id)->channel();

  return ClientChannel();
}


/*!
 * Создание или повторная инициализация вкладки канала.
 *
 * \param id     Идентификатор канала.
 * \param create \b true если необходимо создать канал.
 * \param show   \b true если необходимо выбрать эту вкладку.
 *
 * \return Возвращает указатель на вкладку или 0 в случае ошибки.
 */
ChannelBaseTab *TabWidget::channelTab(const QByteArray &id, bool create, bool show)
{
  ChannelBaseTab *tab = 0;

  if (m_channels.contains(id)) {
    tab = m_channels.value(id);
    create = false;
  }

  ClientChannel channel = ChatClient::channels()->get(id);
  if (!channel)
    return 0;

  if (create) {
    if (channel->type() == SimpleID::UserId)
      tab = new PrivateTab(channel, this);
    else if (channel->type() == SimpleID::ChannelId)
      tab = new ChannelTab(channel, this);

    if (tab) {
      m_channels[id] = tab;
      addTab(tab, channel->name());
      tab->setOnline();
      connect(tab, SIGNAL(actionTriggered(bool)), SLOT(openTab()));
    }
  }

  if (show && tab)
    setCurrentIndex(indexOf(tab));

  return tab;
}


/*!
 * Добавление нового сообщения.
 *
 * \param message Сообщение.
 * \param create  \b true если при необходимости будет создана новая вкладка.
 */
void TabWidget::add(const Message &message, bool create)
{
  if (!m_self)
    return;

  m_self->addImpl(message, create);
}


/*!
 * Проверка состояния вкладки, для принятия решения отображать оповещения или нет.
 *
 * \param id Идентификатор вкладки.
 *
 * \return \b true если окно чата активно и выбрана вкладка с идентификатором равным \p id.
 */
bool TabWidget::isActive(const QByteArray &id)
{
  if (!m_self)
    return false;

  if (!m_self->parentWidget()->isActiveWindow())
    return false;

  AbstractTab *tab = m_self->widget(m_self->currentIndex());
  if (tab && tab->id() == id)
    return true;

  return false;
}


/*!
 * Обработка закрытия вкладки.
 */
void TabWidget::closeTab(int index)
{
  SCHAT_DEBUG_STREAM(this << "closeTab()" << index)

  if (index == -1)
    return;

  AbstractTab *tab = widget(index);

  // Закрытие канала.
  if (m_channels.contains(tab->id()))
    m_channels.remove(tab->id());

  removeTab(index);

  if (tab->isDeleteOnClose())
    QTimer::singleShot(0, tab, SLOT(deleteLater()));
  else
    tab->setOnline(false);

  lastTab();
}



bool TabWidget::event(QEvent *event)
{
  if (event->type() == QEvent::WindowActivate)
    stopAlert();

  return QTabWidget::event(event);
}


void TabWidget::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QTabWidget::changeEvent(event);
}


/*!
 * Контекстное меню для вкладки.
 */
void TabWidget::contextMenuEvent(QContextMenuEvent *event)
{
  int index = m_tabBar->tabAt(event->pos());
  if (index == -1) {
    QTabWidget::contextMenuEvent(event);
    return;
  }

  QMenu menu(this);
  QAction *closeAction = 0;

  if (widget(index)->bindMenu(&menu))
    menu.addSeparator();

  if (tabsClosable())
    closeAction = menu.addAction(SCHAT_ICON(Remove), tr("Close Tab"));

  if (!menu.actions().isEmpty()) {
    QAction *action = menu.exec(event->globalPos());
    if (action == 0)
      return;

    if (action == closeAction)
      closeTab(index);
  }
}


void TabWidget::keyPressEvent(QKeyEvent *event)
{
  if (event->modifiers() == Qt::ControlModifier) {
    if (event->key() == Qt::Key_W) {
      if (currentIndex() != -1 && tabsClosable())
        closeTab(currentIndex());
    }
  }

  QTabWidget::keyPressEvent(event);
}


/*!
 * Закрытие вкладок средней кнопкой мыши.
 */
void TabWidget::mouseReleaseEvent(QMouseEvent *event)
{
  int index = m_tabBar->tabAt(event->pos());
  if (index != -1 && tabsClosable() && event->button() == Qt::MidButton)
    closeTab(index);
  else
    QTabWidget::mouseReleaseEvent(event);
}


void TabWidget::currentChanged(int index)
{
  if (index == -1)
    return;

  AbstractTab *tab = widget(index);
  bool visible = true;
  if (tab->type() == AbstractTab::SettingsType
      || tab->type() == AbstractTab::AboutType
      || tab->type() == AbstractTab::WelcomeType
      || tab->type() == AbstractTab::ProgressType) {
    visible = false;
  }

  ChatCore::setCurrentId(tab->id());

  stopAlert();
  emit pageChanged(tab->type(), visible);
}


void TabWidget::notify(const Notify &notify)
{
  int type = notify.type();

  if (type == Notify::OpenChannel || type == Notify::OpenInfo) {
    ChannelBaseTab *tab = channelTab(notify.data().toByteArray());
    if (!tab)
      return;

    if (type == Notify::OpenInfo)
      tab->chatView()->evaluateJavaScript(LS("Pages.setPage(1);"));
    else
      tab->chatView()->evaluateJavaScript(LS("Pages.setPage(0);"));
  }
  else if (type == Notify::OpenAbout) {
    if (!m_aboutTab)
      m_aboutTab = new AboutTab(this);

    addChatTab(m_aboutTab);
  }
  else if (type == Notify::OpenSettings) {
    if (!m_settingsTab)
      m_settingsTab = new SettingsTab(this);

    m_settingsTab->openUrl(notify.data().toUrl());
    addChatTab(m_settingsTab);
  }
  else if (type == Notify::CopyRequest) {
    ChannelBaseTab *tab = qobject_cast<ChannelBaseTab *>(currentWidget());
    if (!tab)
      return;

    tab->chatView()->copy();
  }
}


/*!
 * Установка индекса на вкладку.
 */
void TabWidget::openTab()
{
  addChatTab(qobject_cast<AbstractTab *>(sender()));
}


void TabWidget::addChannel(const QByteArray &id)
{
  if (SimpleID::typeOf(id) == SimpleID::ChannelId)
    channelTab(id, true, !m_channels.contains(id));
}


/*!
 * Обработка изменения состояния клиента.
 */
void TabWidget::clientStateChanged(int state, int previousState)
{
  if (state == ChatClient::Online && m_welcomeTab) {
    closeTab(indexOf(m_welcomeTab));
  }

  if (previousState == ChatClient::Connecting && m_progressTab) {
    closeTab(indexOf(m_progressTab));
  }

  if (state == ChatClient::Error) {
    QVariantMap error = ChatClient::io()->json().value(LS("error")).toMap();
    int status = error.value(LS("status")).toInt();

    if (status == Notice::Unauthorized && !m_welcomeTab)
      ChatUrls::open(LS("chat://settings/network"));
  }
}


/*!
 * Добавление вкладки и установка её текущей вкладкой.
 * Если вкладка была добавлена ранее, то она просто становится текущей.
 *
 * \param tab Указатель на вкладку.
 */
int TabWidget::addChatTab(AbstractTab *tab)
{
  if (tab == 0)
    return -1;

  int index = indexOf(tab);
  if (index == -1) {
    index = addTab(tab, tab->text());
    tab->setOnline();
  }

  setCurrentIndex(index);
  return index;
}


/*!
 * Добавление нового сообщения.
 */
void TabWidget::addImpl(const Message &message, bool create)
{
  QByteArray id = message.tab();
  if (!Channel::isCompatibleId(id))
    return;

  if (m_channels.contains(id)) {
    m_channels.value(id)->add(message);
  }
  else if (SimpleID::typeOf(id) == SimpleID::ServerId) {
    m_serverTab->chatView()->add(message);
  }
  else if (SimpleID::typeOf(id) == SimpleID::UserId) {
    ChannelBaseTab *tab = channelTab(id, create, false);
    if (tab)
      tab->add(message);
  }
}


/*!
 * Создание панелей инструментов.
 */
void TabWidget::createToolBars()
{
  m_tabsToolBar = new TabsToolBar(this);
  m_mainToolBar = new MainToolBar(this);

  setCornerWidget(m_tabsToolBar, Qt::TopLeftCorner);
  setCornerWidget(m_mainToolBar, Qt::TopRightCorner);
}


/*!
 * \todo Не отображать вкладку уведомлений после подключения к новому серверу.
 */
void TabWidget::lastTab()
{
  if (count() == 0)
    addChatTab(m_serverTab);
}


void TabWidget::retranslateUi()
{
  m_tray->retranslateUi();
}


/*!
 * Отображает вкладку приветствия или прогресса подключения в зависимости от состояния клиента.
 */
void TabWidget::showWelcome()
{
  if (ChatCore::networks()->isAutoConnect()) {
    m_progressTab = new ProgressTab(this);
    addChatTab(m_progressTab);
  }
  else {
    m_welcomeTab = new WelcomeTab(this);
    addChatTab(m_welcomeTab);
  }
}


void TabWidget::stopAlert()
{
  ChannelBaseTab *tab = qobject_cast<ChannelBaseTab *>(currentWidget());
  if (!tab)
    return;

  if (tab->alerts())
    tab->alert(false);
}
