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
#include <QEvent>
#include <QMenu>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>

#include "Channel.h"
#include "debugstream.h"
#include "MessageAdapter.h"
#include "net/packets/message.h"
#include "net/SimpleClient.h"
#include "ui/SoundButton.h"
#include "ui/TabBar.h"
#include "ui/tabs/ChannelTab.h"
#include "ui/tabs/PrivateTab.h"
#include "ui/tabs/UserView.h"
#include "ui/tabs/WelcomeTab.h"
#include "ui/TabWidget.h"
#include "ui/UserUtils.h"
#include "User.h"

TabWidget::TabWidget(SimpleClient *client, QWidget *parent)
  : QTabWidget(parent)
  , m_client(client)
  , m_tabBar(new TabBar(this))
{
  setTabBar(m_tabBar);
  setDocumentMode(true);

  #if !defined(Q_OS_MAC)
  setStyleSheet("QToolBar { margin:0px; border:0px; }");
  #endif

  m_welcomeTab = new WelcomeTab(client, this);
  m_messageAdapter = new MessageAdapter(client);
  addTab(m_welcomeTab, "Welcome");

  createToolBars();
  retranslateUi();

  connect(this, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
  connect(m_client, SIGNAL(join(const QByteArray &, const QByteArray &)), SLOT(join(const QByteArray &, const QByteArray &)));
  connect(m_client, SIGNAL(join(const QByteArray &, const QList<QByteArray> &)), SLOT(join(const QByteArray &, const QList<QByteArray> &)));
  connect(m_client, SIGNAL(part(const QByteArray &, const QByteArray &)), SLOT(part(const QByteArray &, const QByteArray &)));
  connect(m_client, SIGNAL(clientStateChanged(int)), SLOT(clientStateChanged(int)));
  connect(m_client, SIGNAL(userDataChanged(const QByteArray &)), SLOT(updateUserData(const QByteArray &)));
  connect(m_client, SIGNAL(userLeave(const QByteArray &)), SLOT(userLeave(const QByteArray &)));
  connect(m_messageAdapter, SIGNAL(message(int, const MessageData &)), SLOT(message(int, const MessageData &)));
}


AbstractTab *TabWidget::widget(int index) const
{
  return static_cast<AbstractTab*>(QTabWidget::widget(index));
}


QByteArray TabWidget::currentId() const
{
  return widget(currentIndex())->id();
}


void TabWidget::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QTabWidget::changeEvent(event);
}


void TabWidget::addPrivateTab(const QByteArray &id)
{
  if (m_client->userId() == id)
    return;

  privateTab(id, true, true);
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
  if (tab->type() == AbstractTab::ChannelType) {
    if (m_channels.contains(tab->id())) {
      m_channels.remove(tab->id());
    }

    m_client->part(tab->id());
    removeTab(index);
    QTimer::singleShot(0, tab, SLOT(deleteLater()));
  }
  // Закрытие приватного разговора.
  else if (tab->type() == AbstractTab::PrivateType) {
    if (m_talks.contains(tab->id())) {
      m_talks.remove(tab->id());
    }

    removeTab(index);
    QTimer::singleShot(0, tab, SLOT(deleteLater()));
  }

  showWelcome();
}


void TabWidget::hideMainMenu()
{
  SCHAT_DEBUG_STREAM(this << "hideMainMenu()")
  m_mainMenu->clear();
  m_channelsMenu->clear();
  m_talksMenu->clear();
}


/*!
 * Установка индекса на вкладку.
 */
void TabWidget::openTab()
{
  AbstractTab *tab = qobject_cast<AbstractTab *>(sender());
  if (tab) {
    setCurrentWidget(tab);
  }
}


void TabWidget::showMainMenu()
{
  SCHAT_DEBUG_STREAM(this << "showMainMenu()")

  // Создание меню каналов.

  QList<QAction *> channels;
  QList<QAction *> talks;

  for (int i = 0; i < count(); ++i) {
    AbstractTab *tab = widget(i);
    if (tab->type() == AbstractTab::ChannelType)
      channels.append(tab->action());
    else if (tab->type() == AbstractTab::PrivateType)
      talks.append(tab->action());
  }

  if (!channels.isEmpty()) {
    m_channelsMenu->addActions(channels);
    m_mainMenu->addMenu(m_channelsMenu);
  }

  if (!talks.isEmpty()) {
    m_talksMenu->addActions(talks);
    m_mainMenu->addMenu(m_talksMenu);
  }
}


/*!
 * Обработка изменения состояния клиента.
 */
void TabWidget::clientStateChanged(int state)
{
  SCHAT_DEBUG_STREAM(this << "clientStateChanged()" << state);

  if (state == SimpleClient::ClientOnline)
    return;

  foreach (ChannelTab *tab, m_channels) {
    tab->setOnline(false);
    displayChannelUserCount(tab->id());
  }

  foreach (PrivateTab *tab, m_talks) {
    tab->setOnline(false);
  }
}


/*!
 * \param channelId Идентификатор канала.
 * \param userId    Идентификатор пользователя.
 * \param option    Опция добавления пользователя.
 */
void TabWidget::join(const QByteArray &channelId, const QByteArray &userId, int option)
{
  // Принудительная сортировка списка пользователей, без добавления нового пользователя,
  // используется при завершении синхронизации канала.
  if (option & UserView::NoSort && userId.isEmpty()) {
    ChannelTab *tab = m_channels.value(channelId);
    if (tab) {
      tab->userView()->add(0);
      displayChannelUserCount(channelId);
    }

    return;
  }

  Channel *chan = m_client->channel(channelId);
  if (!chan)
    return;

  // Добавления собственного профиля в канал.
  if (userId.isEmpty()) {
    createChannelTab(channelId);
    return;
  }

  ChannelTab *tab = m_channels.value(channelId);
  User *user = m_client->user(userId);

  if (tab && user) {
    privateTab(user->id(), false);

    if (!tab->userView()->add(user, option))
      return;

    if (option & UserView::NoSort)
      return;

    displayChannelUserCount(channelId);
    tab->chatView()->appendRawText(tr("<b>%1</b> joined to <b>%2</b>").arg(user->nick()).arg(chan->name()));
  }
}


void TabWidget::join(const QByteArray &channelId, const QList<QByteArray> &usersId)
{
  for (int i = 0; i < usersId.size(); ++i) {
    join(channelId, usersId.at(i), UserView::NoSort);
  }
}


/*!
 * Обработка нового сообщения.
 */
void TabWidget::message(int status, const MessageData &data)
{
  if (SimpleID::typeOf(data.senderId) != SimpleID::UserId)
    return;

  int type = SimpleID::typeOf(data.destId);

  if (type == SimpleID::ChannelId) {
    User *user = m_client->user(data.senderId);
    if (!user)
      return;

    ChannelTab *tab = m_channels.value(data.destId);
    if (tab) {
      tab->chatView()->append(status, user, data);
    }
  }
  else if (type == SimpleID::UserId) {
    QByteArray id;
    User *user = 0;

    if (status & MessageAdapter::IncomingMessage) {
      id = data.senderId;
      user = m_client->user(id);
    }
    else if (status & MessageAdapter::OutgoingMessage) {
      id = data.destId;
      user = m_client->user();
    }
    else
      return;

    if (!user)
      return;

    PrivateTab *tab = privateTab(id);
    if (tab) {
      tab->chatView()->append(status, user, data);
    }
  }
}


void TabWidget::part(const QByteArray &channelId, const QByteArray &userId)
{
  ChannelTab *tab = m_channels.value(channelId);
  User *user = m_client->user(userId);
  if (tab && user) {
    tab->chatView()->appendRawText(tr("<b>%1</b> left").arg(user->nick()));
    tab->userView()->remove(userId);
    displayChannelUserCount(channelId);
  }
}


/*!
 * Обновление информации о пользователе.
 *
 * \param userId Идентификатор пользователя, данные которого изменились.
 */
void TabWidget::updateUserData(const QByteArray &userId)
{
  User *user = m_client->user(userId);
  if (!user)
    return;

  QList<QByteArray> channels = user->ids(SimpleID::ChannelListId);
  foreach(QByteArray id, channels) {
    ChannelTab *tab = m_channels.value(id);
    if (tab) {
      tab->userView()->update(user);
    }
  }

  PrivateTab *tab = m_talks.value(userId);
  if (!tab)
    return;

  tab->update(user);
}


void TabWidget::userLeave(const QByteArray &userId)
{
  PrivateTab *tab = m_talks.value(userId);
  if (tab)
    tab->setOnline(false);
}


/*!
 * Создание или повторная инициализация вкладки канала.
 *
 * \param id Идентификатор канала.
 *
 * \return Возвращает указатель на вкладку или 0 в случае ошибки.
 */
ChannelTab *TabWidget::createChannelTab(const QByteArray &id)
{
  Channel *channel = m_client->channel(id);
  if (!channel)
    return 0;

  ChannelTab *tab = 0;

  if (!m_channels.contains(id)) {
    tab = new ChannelTab(id, this);
    m_channels.insert(id, tab);
    setCurrentIndex(addTab(tab, channel->name()));

    connect(tab, SIGNAL(actionTriggered(bool)), SLOT(openTab()));
    connect(tab->userView(), SIGNAL(addTab(const QByteArray &)), SLOT(addPrivateTab(const QByteArray &)));
  }
  else {
    tab = m_channels.value(id);
  }

  tab->setOnline();
  tab->chatView()->appendRawText(tr("you're joined to <b>%1</b>").arg(channel->name()));
  tab->userView()->add(m_client->user(), UserView::SelfNick);
  tab->action()->setText(channel->name());

  showWelcome();
  return tab;
}


/*!
 * Открытие или создание вкладки для приватного разговора.
 *
 * \param id     Идентификатор пользователя.
 * \param create true для создания вкладки, если она не была создана до этого.
 * \param show   true если надо уставить текущий индекс на эту вкладку.
 *
 * \return Возвращает указатель на вкладку или 0 в случае ошибки.
 */
PrivateTab *TabWidget::privateTab(const QByteArray &id, bool create, bool show)
{
  PrivateTab *tab = 0;

  if (m_talks.contains(id)) {
    tab = m_talks.value(id);
    create = false;
  }

  if (create) {
    User *user = m_client->user(id);
    if (!user)
      return 0;

    tab = new PrivateTab(user, this);
    m_talks.insert(id, tab);
    addTab(tab, user->nick());
    tab->setOnline();

    connect(tab, SIGNAL(actionTriggered(bool)), SLOT(openTab()));
  }
  else if (tab) {
    tab->update(m_client->user(id));
  }

  if (show && tab) {
    setCurrentIndex(indexOf(tab));
  }

  return tab;
}


/*!
 * Создание панелей инструментов.
 */
void TabWidget::createToolBars()
{
  // Левый виджет.
  m_leftToolBar = new QToolBar(this);
  m_leftToolBar->setIconSize(QSize(16, 16));

  m_menuButton = new QToolButton(this);
  m_menuButton->setIcon(QIcon(":/images/main-tab-menu.png"));
  m_menuButton->setAutoRaise(true);
  m_menuButton->setPopupMode(QToolButton::InstantPopup);

  m_leftToolBar->addWidget(m_menuButton);

  m_mainMenu = new QMenu(this);
  m_menuButton->setMenu(m_mainMenu);

  m_channelsMenu = new QMenu(this);
  m_channelsMenu->setIcon(QIcon(":/images/channel.png"));

  m_talksMenu = new QMenu(this);
  m_talksMenu->setIcon(QIcon(":/images/users.png"));

  connect(m_mainMenu, SIGNAL(aboutToHide()), SLOT(hideMainMenu()));
  connect(m_mainMenu, SIGNAL(aboutToShow()), SLOT(showMainMenu()));

  // Правый виджет.
  m_rightToolBar = new QToolBar(this);
  m_rightToolBar->setIconSize(QSize(16, 16));

  m_settingsMenu = new QMenu(this);
  // \todo isNewYear().
  m_aboutAction = m_settingsMenu->addAction(QIcon(":/images/schat16.png"), "", this, SIGNAL(requestAbout()));
  m_settingsMenu->addSeparator();
  m_quitAction = m_settingsMenu->addAction(QIcon(":/images/quit.png"), "", this, SIGNAL(requestQuit()));

  m_settingsButton = new QToolButton(this);
  m_settingsButton->setIcon(QIcon(":/images/settings.png"));
  m_settingsButton->setAutoRaise(true);
  m_settingsButton->setPopupMode(QToolButton::InstantPopup);
  m_settingsButton->setMenu(m_settingsMenu);

  m_soundButton = new SoundButton(false, this);

  m_rightToolBar->addWidget(m_settingsButton);
  m_rightToolBar->addWidget(m_soundButton);

  setCornerWidget(m_leftToolBar, Qt::TopLeftCorner);
  setCornerWidget(m_rightToolBar, Qt::TopRightCorner);
}


/*!
 * Отображение в заголовке вкладки числа пользователей, если их больше 1.
 *
 * \param id Идентификатор канала.
 * \bug После добавления класса ClientOfflineCache, при отключении от сервера число пользователей не обнуляется.
 */
void TabWidget::displayChannelUserCount(const QByteArray &id)
{
  ChannelTab *tab = m_channels.value(id);
  Channel *chan = m_client->channel(id);

  if (tab && chan) {
    int index = indexOf(tab);
    if (chan->userCount() > 1)
      setTabText(index, QString("%1 (%2)").arg(chan->name()).arg(chan->userCount()));
    else
      setTabText(index, chan->name());
  }
}


void TabWidget::retranslateUi()
{
  m_menuButton->setToolTip(tr("Menu"));
  m_settingsButton->setToolTip(tr("Preferences"));
  m_aboutAction->setText(tr("About..."));
  m_quitAction->setText(tr("Quit"));
  m_channelsMenu->setTitle(tr("Channels"));
  m_talksMenu->setTitle(tr("Talks"));
}


void TabWidget::showWelcome()
{
  int index = indexOf(m_welcomeTab);

  if (m_channels.size()) {
    removeTab(index);
  }
  else {
    addTab(m_welcomeTab, tr("Welcome"));
  }
}
