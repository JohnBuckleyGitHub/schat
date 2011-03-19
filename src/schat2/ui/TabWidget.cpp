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
#include "net/SimpleClient.h"
#include "ui/SoundButton.h"
#include "ui/TabBar.h"
#include "ui/tabs/ChannelTab.h"
#include "ui/tabs/UserView.h"
#include "ui/TabWidget.h"
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

  createToolBars();
  retranslateUi();

  connect(this, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
  connect(m_client, SIGNAL(join(const QByteArray &, const QByteArray &)), SLOT(join(const QByteArray &, const QByteArray &)));
  connect(m_client, SIGNAL(join(const QByteArray &, const QList<QByteArray> &)), SLOT(join(const QByteArray &, const QList<QByteArray> &)));
  connect(m_client, SIGNAL(part(const QByteArray &, const QByteArray &)), SLOT(part(const QByteArray &, const QByteArray &)));
  connect(m_client, SIGNAL(clientStateChanged(int)), SLOT(clientStateChanged(int)));
}


AbstractTab *TabWidget::widget(int index) const
{
  return static_cast<AbstractTab*>(QTabWidget::widget(index));
}


void TabWidget::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QTabWidget::changeEvent(event);
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
  if (tab->type() == AbstractTab::ChannelType) {
    if (m_channels.contains(tab->id())) {
      m_channels.remove(tab->id());
    }

    m_client->part(tab->id());
    removeTab(index);
    QTimer::singleShot(0, tab, SLOT(deleteLater()));
  }
}


void TabWidget::hideMainMenu()
{
  SCHAT_DEBUG_STREAM(this << "hideMainMenu()")
  m_mainMenu->clear();
  m_channelsMenu->clear();
}


/*!
 * Установка индекса на этот канал.
 */
void TabWidget::openChannel()
{
  ChannelTab *tab = qobject_cast<ChannelTab *>(sender());
  if (tab) {
    setCurrentWidget(tab);
  }
}


void TabWidget::showMainMenu()
{
  SCHAT_DEBUG_STREAM(this << "showMainMenu()")

  // Создание меню каналов.
  if (m_channels.size()) {
    for (int i = 0; i < count(); ++i) {
      AbstractTab *tab = widget(i);
      if (tab->type() == AbstractTab::ChannelType)
        m_channelsMenu->addAction(tab->action());
    }
    m_mainMenu->addMenu(m_channelsMenu);
  }
}


void TabWidget::clientStateChanged(int state)
{
  SCHAT_DEBUG_STREAM(this << "clientStateChanged()" << state);

  bool online = state == SimpleClient::ClientOnline;

  foreach (ChannelTab *tab, m_channels) {
    tab->setOnline(online);
    if (!online) {
      displayChannelUserCount(tab->id());
    }
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

  if (userId.isEmpty() && m_channels.contains(channelId))
    return;

  Channel *chan = m_client->channel(channelId);
  if (!chan)
    return;

  // Добавления собственного профиля в канал.
  if (userId.isEmpty()) {
    createChannelTab(chan);
    return;
  }

  ChannelTab *tab = m_channels.value(channelId);
  User *user = m_client->user(userId);

  if (tab && user) {
    if (!tab->userView()->add(user, option))
      return;

    if (option & UserView::NoSort)
      return;

    displayChannelUserCount(channelId);
    tab->appendRawText(tr("<b>%1</b> joined to <b>%2</b>").arg(user->nick()).arg(chan->name()));
  }
}


void TabWidget::join(const QByteArray &channelId, const QList<QByteArray> &usersId)
{
  for (int i = 0; i < usersId.size(); ++i) {
    join(channelId, usersId.at(i), UserView::NoSort);
  }
}


void TabWidget::part(const QByteArray &channelId, const QByteArray &userId)
{
  ChannelTab *tab = m_channels.value(channelId);
  User *user = m_client->user(userId);
  if (tab && user) {
    tab->appendRawText(tr("<b>%1</b> left").arg(user->nick()));
    tab->userView()->remove(userId);
    displayChannelUserCount(channelId);
  }
}


/*!
 * Создание вкладки для канала.
 */
void TabWidget::createChannelTab(Channel *channel)
{
  ChannelTab *tab = new ChannelTab(channel->id(), this);
  m_channels.insert(channel->id(), tab);
  setCurrentIndex(addTab(tab, channel->name()));

  tab->setOnline();
  tab->appendRawText(tr("you're joined to <b>%1</b>").arg(channel->name()));
  tab->userView()->add(m_client->user(), UserView::SelfNick);
  tab->action()->setText(channel->name());

  connect(tab, SIGNAL(actionTriggered(bool)), SLOT(openChannel()));
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
 */
void TabWidget::displayChannelUserCount(const QByteArray &channelId)
{
  ChannelTab *tab = m_channels.value(channelId);
  Channel *chan = m_client->channel(channelId);

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
  m_channelsMenu->setTitle(tr("Groups"));
}
