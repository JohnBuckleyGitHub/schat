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
#include <QTimer>
#include <QToolBar>
#include <QToolButton>

#include "debugstream.h"

#include "actions/MenuBuilder.h"
#include "Channel.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/SimpleClient.h"
#include "messages/AbstractMessage.h"
#include "net/packets/message.h"
#include "NetworkManager.h"
#include "ui/SoundButton.h"
#include "ui/TabBar.h"
#include "ui/tabs/AboutTab.h"
#include "ui/tabs/AlertTab.h"
#include "ui/tabs/ChannelTab.h"
#include "ui/tabs/ChatView.h"
#include "ui/tabs/PrivateTab.h"
#include "ui/tabs/ProgressTab.h"
#include "ui/tabs/SettingsTab.h"
#include "ui/tabs/UserView.h"
#include "ui/tabs/WelcomeTab.h"
#include "ui/TabWidget.h"
#include "ui/TrayIcon.h"
#include "ui/UserUtils.h"
#include "User.h"

TabWidget::TabWidget(QWidget *parent)
  : QTabWidget(parent)
  , m_core(ChatCore::i())
  , m_client(ChatCore::i()->client())
  , m_tabBar(new TabBar(this))
{
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
  QWebSettings::globalSettings()->setFontFamily(QWebSettings::StandardFont, fontInfo().family());
  QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, SCHAT_OPTION("DeveloperExtras").toBool());

  m_alertTab = new AlertTab(this);
  m_alertTab->setVisible(false);

  showWelcome();

  m_tray = new TrayIcon(this);
  QTimer::singleShot(0, m_tray, SLOT(show()));

  createToolBars();
  retranslateUi();

  connect(this, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
  connect(this, SIGNAL(currentChanged(int)), SLOT(currentChanged(int)));
  connect(m_client, SIGNAL(join(const QByteArray &, const QByteArray &)), SLOT(join(const QByteArray &, const QByteArray &)));
  connect(m_client, SIGNAL(synced(const QByteArray &)), SLOT(synced(const QByteArray &)));
  connect(m_client, SIGNAL(part(const QByteArray &, const QByteArray &)), SLOT(part(const QByteArray &, const QByteArray &)));
  connect(m_client, SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged(int, int)));
  connect(m_client, SIGNAL(userDataChanged(const QByteArray &)), SLOT(updateUserData(const QByteArray &)));
  connect(m_client, SIGNAL(userLeave(const QByteArray &)), SLOT(userLeave(const QByteArray &)));
  connect(ChatCore::i(), SIGNAL(message(const AbstractMessage &)), SLOT(message(const AbstractMessage &)));
  connect(ChatCore::i(), SIGNAL(notify(int, const QVariant &)), SLOT(notify(int, const QVariant &)));
  connect(m_alertTab, SIGNAL(actionTriggered(bool)), SLOT(openTab()));
}


AbstractTab *TabWidget::widget(int index) const
{
  return static_cast<AbstractTab*>(QTabWidget::widget(index));
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
  MenuBuilder *builder = widget(index)->menu();

  if (builder) {
    builder->bind(&menu);
    menu.addSeparator();
  }

  if (tabsClosable()) {
    closeAction = menu.addAction(tr("Close Tab"));
  }

  if (!menu.actions().isEmpty()) {
    QAction *action = menu.exec(event->globalPos());
    if (action == 0)
      return;

    if (action == closeAction)
      closeTab(index);
  }

  if (builder)
    builder->deleteLater();
}


/*!
 * Закрытие вкладок средней кнопкой мыши.
 */
void TabWidget::mouseReleaseEvent(QMouseEvent *event)
{
  int index = m_tabBar->tabAt(event->pos());
  if (index != -1 && tabsClosable() && event->button() == Qt::MiddleButton)
    closeTab(index);
  else
    QTabWidget::mouseReleaseEvent(event);
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
  if (tab->type() == AbstractTab::ChannelType && m_channels.contains(tab->id())) {
    m_channels.remove(tab->id());
  }
  // Закрытие приватного разговора.
  else if (tab->type() == AbstractTab::PrivateType && m_talks.contains(tab->id())) {
    m_talks.remove(tab->id());
  }

  removeTab(index);
  if (tab->isDeleteOnClose())
    QTimer::singleShot(0, tab, SLOT(deleteLater()));
  else
    tab->setOnline(false);

  lastTab();
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

  m_core->setCurrentId(tab->id());

  stopAlert();
  emit pageChanged(tab->type(), visible);
}


void TabWidget::hideMainMenu()
{
  m_mainMenu->clear();
  m_channelsMenu->clear();
  m_talksMenu->clear();
}


void TabWidget::notify(int notice, const QVariant &data)
{
  Q_UNUSED(data)

  if (notice == ChatCore::AboutNotice) {
    if (!m_aboutTab)
      m_aboutTab = new AboutTab(this);

    addChatTab(m_aboutTab);
  }
  else if (notice == ChatCore::SettingsNotice) {
    if (!m_settingsTab)
      m_settingsTab = new SettingsTab(this);

    addChatTab(m_settingsTab);
  }
  else if (notice == ChatCore::AddPrivateTab) {
    addPrivateTab(data.toByteArray());
  }
}


/*!
 * Установка индекса на вкладку.
 */
void TabWidget::openTab()
{
  addChatTab(qobject_cast<AbstractTab *>(sender()));
}


void TabWidget::showMainMenu()
{
  // Создание меню каналов.
  QList<QAction *> channels;
  QList<QAction *> talks;

  AbstractTab *currentTab = widget(currentIndex());

  for (int i = 0; i < count(); ++i) {
    AbstractTab *tab = widget(i);
    if (tab->type() == AbstractTab::ChannelType)
      channels.append(tab->action());
    else if (tab->type() == AbstractTab::PrivateType)
      talks.append(tab->action());

    tab->action()->setChecked(currentTab == tab);
  }

  bool separator = false;

  if (!channels.isEmpty()) {
    m_channelsMenu->addActions(channels);
    m_mainMenu->addMenu(m_channelsMenu);
    separator = true;
  }

  if (!talks.isEmpty()) {
    m_talksMenu->addActions(talks);
    m_mainMenu->addMenu(m_talksMenu);
    separator = true;
  }

  if (separator)
    m_mainMenu->addSeparator();

  m_mainMenu->addAction(m_alertTab->action());
  m_alertTab->action()->setChecked(currentTab == m_alertTab);
}


/*!
 * Обработка входа пользователя в канал.
 *
 * \param channelId Идентификатор канала.
 * \param userId    Идентификатор пользователя.
 */
void TabWidget::join(const QByteArray &channelId, const QByteArray &userId)
{
  ClientChannel chan = m_client->channel(channelId);
  if (!chan)
    return;

  ChannelTab *tab = m_channels.value(channelId);
  ClientUser user = m_client->user(userId);

  if (tab && user) {
    privateTab(user->id(), false);

    if (!tab->userView()->add(user))
      return;

    displayChannelUserCount(channelId);
    addJoinMsg(user->id(), channelId, tab);
  }
}


void TabWidget::part(const QByteArray &channelId, const QByteArray &userId)
{
  ChannelTab *tab = m_channels.value(channelId);
  ClientUser user = m_client->user(userId);
  if (tab && user) {

    addQuitMsg(userId, channelId, tab);
    tab->userView()->remove(userId);
    displayChannelUserCount(channelId);
  }
}


/*!
 * Обработка завершения синхронизации канала.
 *
 * \param channelId Идентификатор канала.
 */
void TabWidget::synced(const QByteArray &channelId)
{
  ClientChannel channel = m_client->channel(channelId);
  if (!channel)
    return;

  ChannelTab *tab = channelTab(channelId);

  if (channel->userCount() == 1) {
    tab->userView()->sort();
    return;
  }

  QList<QByteArray> users = channel->users();
  for (int i = 0; i < users.size(); ++i) {
    ClientUser user = m_client->user(users.at(i));
    if (!user)
      continue;

    privateTab(user->id(), false);
    tab->userView()->add(user);
  }

  tab->userView()->sort();
}


/*!
 * Обработка изменения состояния клиента.
 */
void TabWidget::clientStateChanged(int state, int previousState)
{
  SCHAT_DEBUG_STREAM(this << "clientStateChanged()" << state);

  if (state == SimpleClient::ClientOnline)
    return;

  QByteArray id = m_client->userId();
  bool quitMsg = state == SimpleClient::ClientOffline && previousState == SimpleClient::ClientOnline;

  foreach (ChannelTab *tab, m_channels) {
    tab->setOnline(false);
    displayChannelUserCount(tab->id());

    if (quitMsg)
      addQuitMsg(id, tab->id(), tab);
  }

  foreach (PrivateTab *tab, m_talks) {
    tab->setOnline(false);

    if (quitMsg)
      addQuitMsg(id, id, tab);
  }

  if (state == SimpleClient::ClientOffline) {
    closeWelcome();
  }
}


/*!
 * Отображение сообщения.
 *
 * \param data Абстрактное сообщение.
 */
void TabWidget::message(const AbstractMessage &data)
{
  ChatViewTab *tab = 0;

  if (data.destId().isEmpty()) {
    tab = m_alertTab;
  }
  else {
    int type = SimpleID::typeOf(data.destId());

    if (type == SimpleID::ChannelId) {
      if (!data.senderId().isEmpty() && !m_client->user(data.senderId()))
        return;

      tab = m_channels.value(data.destId());
    }
    else if (type == SimpleID::UserId) {
      QByteArray id;

      if (data.direction() == AbstractMessage::IncomingDirection)
        id = data.senderId();
      else
        id = data.destId();

      tab = privateTab(id);
    }
  }

  if (!tab)
    return;

  message(tab, data);
}


/*!
 * Обновление информации о пользователе.
 *
 * \param userId Идентификатор пользователя, данные которого изменились.
 */
void TabWidget::updateUserData(const QByteArray &userId)
{
  ClientUser user = m_client->user(userId);
  if (!user)
    return;

  QList<QByteArray> channels = user->channels();
  foreach(QByteArray id, channels) {
    ChannelTab *tab = m_channels.value(id);
    if (tab) {
      tab->userView()->update(user);
    }
  }

  PrivateTab *tab = privateTab(userId, false);
  if (tab)
    tab->update(user);
}


void TabWidget::userLeave(const QByteArray &userId)
{
  if (!m_talks.contains(userId))
    return;

  addQuitMsg(userId, userId);
}


/*!
 * Создание или повторная инициализация вкладки канала.
 *
 * \param id Идентификатор канала.
 *
 * \return Возвращает указатель на вкладку или 0 в случае ошибки.
 */
ChannelTab *TabWidget::channelTab(const QByteArray &id)
{
  ClientChannel channel = m_client->channel(id);
  if (!channel)
    return 0;

  ChannelTab *tab = 0;

  if (!m_channels.contains(id)) {
    tab = new ChannelTab(channel, this);
    m_channels.insert(id, tab);
    setCurrentIndex(addTab(tab, channel->name()));

    connect(tab, SIGNAL(actionTriggered(bool)), SLOT(openTab()));
  }
  else {
    tab = m_channels.value(id);
  }

  tab->setOnline();
  tab->userView()->add(m_client->user());
  tab->action()->setText(channel->name());

  addJoinMsg(m_client->userId(), id, tab);
  closeWelcome();
  return tab;
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

  ClientUser user = UserUtils::user(id);
  if (!user)
    return 0;

  if (create) {
    tab = new PrivateTab(user, this);
    m_talks.insert(id, tab);
    addTab(tab, user->nick());
    tab->setOnline();

    connect(tab, SIGNAL(actionTriggered(bool)), SLOT(openTab()));
  }
  else if (tab) {
    bool addJoin = tab->user() != user;
    tab->update(m_client->user(id));

    if (addJoin)
      addJoinMsg(id, id, tab);
  }

  if (show && tab) {
    setCurrentIndex(indexOf(tab));
  }

  return tab;
}


void TabWidget::addJoinMsg(const QByteArray &userId, const QByteArray &destId, ChatViewTab *tab)
{
  ClientUser user = m_client->user(userId);
  if (!user)
    return;

  QString text;
  if (user->gender() == User::Female)
    text = tr("has joined", "Female");
  else
    text = tr("has joined", "Male");

  addServiceMsg(userId, destId, text, tab);
}


/*!
 * Сообщение о выходе пользователя из канала или чата.
 */
void TabWidget::addQuitMsg(const QByteArray &userId, const QByteArray &destId, ChatViewTab *tab)
{
  ClientUser user = m_client->user(userId);
  if (!user)
    return;

  QString text;
  if (user->status() == User::OfflineStatus || m_client->userId() == userId) {
    if (user->gender() == User::Female)
      text = tr("has quit chat", "Female");
    else
      text = tr("has quit chat", "Male");
  }
  else {
    if (user->gender() == User::Female)
      text = tr("has left", "Female");
    else
      text = tr("has left", "Male");
  }

  addServiceMsg(userId, destId, text, tab);
}


/*!
 * Добавление сервисного сообщения.
 */
void TabWidget::addServiceMsg(const QByteArray &userId, const QByteArray &destId, const QString &text, ChatViewTab *tab)
{
  MessageData data(userId, destId, text);
  data.timestamp = m_client->timestamp();

  AbstractMessage msg(QLatin1String("service-type"), data);
  msg.setPriority(AbstractMessage::LowPriority);

  message(tab, msg);
}


/*!
 * Закрытие вкладки приветствия или прогресса подключения.
 * Если процесс подключения был прерван и отображается только вкладка прогресса
 * подключения, то она будет заменена на вкладку приветствия.
 *
 * \sa showWelcome().
 */
void TabWidget::closeWelcome()
{
  if (m_welcomeTab) {
    int index = indexOf(m_welcomeTab);
    if (index != -1)
      closeTab(index);
  }

  if (m_progressTab) {
    int index = indexOf(m_progressTab);
    if (index != -1) {
      if (count() == 1 && m_client->clientState() == SimpleClient::ClientOffline) {
        if (!m_welcomeTab)
          m_welcomeTab = new WelcomeTab(this);
        addChatTab(m_welcomeTab);
      }

      closeTab(index);
    }
  }
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
  m_menuButton->setIcon(SCHAT_ICON(MainTabMenuIcon));
  m_menuButton->setAutoRaise(true);
  m_menuButton->setPopupMode(QToolButton::InstantPopup);

  m_leftToolBar->addWidget(m_menuButton);

  m_mainMenu = new QMenu(this);
  m_menuButton->setMenu(m_mainMenu);

  m_channelsMenu = new QMenu(this);
  m_channelsMenu->setIcon(SCHAT_ICON(ChannelIcon));

  m_talksMenu = new QMenu(this);
  m_talksMenu->setIcon(SCHAT_ICON(UsersIcon));

  connect(m_mainMenu, SIGNAL(aboutToHide()), SLOT(hideMainMenu()));
  connect(m_mainMenu, SIGNAL(aboutToShow()), SLOT(showMainMenu()));

  // Правый виджет.
  m_rightToolBar = new QToolBar(this);
  m_rightToolBar->setIconSize(QSize(16, 16));

  m_settingsMenu = new QMenu(this);
  // \todo isNewYear().
  m_settingsMenu->addAction(m_tray->settingsAction());
  m_settingsMenu->addAction(m_tray->aboutAction());
  m_settingsMenu->addSeparator();
  m_settingsMenu->addAction(m_tray->quitAction());

  m_settingsButton = new QToolButton(this);
  m_settingsButton->setIcon(SCHAT_ICON(SettingsIcon));
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
 */
void TabWidget::displayChannelUserCount(const QByteArray &id)
{
  ChannelTab *tab = m_channels.value(id);
  ClientChannel chan = m_client->channel(id);

  if (tab && chan) {
    int index = indexOf(tab);
    if (chan->userCount() > 1)
      setTabText(index, QString("%1 (%2)").arg(chan->name()).arg(chan->userCount()));
    else
      setTabText(index, chan->name());
  }
}


void TabWidget::lastTab()
{
  if (count() == 0)
    addChatTab(m_alertTab);
}


void TabWidget::message(ChatViewTab *tab, const AbstractMessage &data)
{
  if (!tab) {
    message(data);
    return;
  }

  tab->chatView()->evaluateJavaScript(data.js());

  if (data.priority() < AbstractMessage::NormalPriority)
    return;

  int index = indexOf(tab);
  bool alert = false;
  if (index != currentIndex() || !parentWidget()->isActiveWindow()) {
    alert = true;
    tab->alert();
    m_tray->alert();

    if (!m_alerts.contains(tab))
      m_alerts.append(tab);
  }
}


void TabWidget::retranslateUi()
{
  m_tray->retranslateUi();

  m_menuButton->setToolTip(tr("Menu"));
  m_settingsButton->setToolTip(tr("Preferences"));
  m_channelsMenu->setTitle(tr("Channels"));
  m_talksMenu->setTitle(tr("Talks"));
}


/*!
 * Отображает вкладку приветствия или прогресса подключения в зависимости от состояния клиента.
 *
 * Вкладка прогресса подключения будет отображена в следующих случаях:
 * - Конфигурационная опция AutoConnect равна true.
 * - Статус пользователя не равен User::OfflineStatus.
 * - Есть сохранённые сети.
 *
 * \sa closeWelcome().
 */
void TabWidget::showWelcome()
{
  if (SCHAT_OPTION("AutoConnect").toBool() && m_client->user()->status() != User::OfflineStatus && ChatCore::i()->networks()->count()) {
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
  ChatViewTab *tab = qobject_cast<ChatViewTab *>(currentWidget());
  if (!tab)
    return;

  m_alerts.removeAll(tab);
  if (tab->alerts())
    tab->alert(false);

  if (m_alerts.isEmpty())
    m_tray->alert(false);
}
