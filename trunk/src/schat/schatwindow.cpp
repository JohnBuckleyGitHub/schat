/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui>
#include <QtNetwork>

#include "aboutdialog.h"
#include "abstractprofile.h"
#include "mainchannel.h"
#include "protocol.h"
#include "schatwindow.h"
#include "settings.h"
#include "settingsdialog.h"
#include "tab.h"
#include "version.h"
#include "welcomedialog.h"
#include "widget/sendwidget.h"

/*!
 * \class SChatWindow
 * \brief Главное окно чата.
 */

/*!
 * \brief Конструктор класса SChatWindow.
 */
SChatWindow::SChatWindow(QWidget *parent)
  : QMainWindow(parent)
{
  m_profile     = new AbstractProfile(this);
  m_settings    = new Settings(qApp->applicationDirPath() + "/schat.conf", m_profile, this);
  m_send        = new SendWidget(m_settings, this);
  centralWidget = new QWidget(this);
  splitter      = new QSplitter(centralWidget);
  m_tabs        = new QTabWidget(splitter);
  rightWidget   = new QWidget(splitter);
  listView      = new QListView(rightWidget);
  rightLayout   = new QVBoxLayout(rightWidget);
  mainLayout    = new QVBoxLayout(centralWidget);
  sendLayout    = new QHBoxLayout;
  toolsLayout   = new QHBoxLayout;
  statusbar     = new QStatusBar(this);
  statusLabel   = new QLabel(this);
  noticeTimer   = new QTimer(this);
  noticeTimer->setInterval(800);
  
  splitter->addWidget(m_tabs);
  splitter->addWidget(rightWidget);
  splitter->setStretchFactor(0, 4);
  splitter->setStretchFactor(1, 1);
  
  rightLayout->addLayout(toolsLayout);
  rightLayout->addWidget(listView);
  rightLayout->setMargin(0);
  rightLayout->setSpacing(4);

  mainLayout->addWidget(splitter);
  mainLayout->addWidget(m_send);
  mainLayout->setMargin(4);
  mainLayout->setSpacing(1);
  mainLayout->setStretchFactor(splitter, 999);
  mainLayout->setStretchFactor(m_send, 1);
  
  setCentralWidget(centralWidget);
  setStatusBar(statusbar);
  statusbar->addWidget(statusLabel, 1);
  statusLabel->setText(tr("Не подключено"));
  
  setWindowTitle(tr("IMPOMEZIA Simple Chat"));
  
  m_tabs->setElideMode(Qt::ElideRight);
  listView->setFocusPolicy(Qt::NoFocus);
  listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  listView->setModel(&model);

  m_settings->read();
  restoreGeometry();

  #ifdef SCHAT_UPDATE
  m_updateTimer = new QTimer(this);
  m_updateTimer->setInterval(m_settings->getInt("Updates/CheckInterval") * 60 * 1000);
  connect(m_updateTimer, SIGNAL(timeout()), SLOT(update()));
  #endif

  createActions();
  createCornerWidgets();
  createToolButtons();
  createTrayIcon();
  createService();

  connect(m_send, SIGNAL(sendMsg(const QString &)), SLOT(sendMsg(const QString &)));
  connect(listView, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(addTab(const QModelIndex &)));
  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
  connect(noticeTimer, SIGNAL(timeout()), SLOT(notice()));
  connect(m_tabs, SIGNAL(currentChanged(int)), SLOT(resetTabNotice(int)));
  connect(trayIcon, SIGNAL(messageClicked()), SLOT(messageClicked()));
  connect(m_settings, SIGNAL(changed(int)), SLOT(settingsChanged(int)));
  
  mainChannel = new MainChannel(m_settings, this);
  mainChannel->icon.addFile(":/images/main.png");
  m_tabs->setCurrentIndex(m_tabs->addTab(mainChannel, tr("Общий")));
  m_tabs->setTabIcon(0, mainChannel->icon);
  
  if (!m_settings->getBool("HideWelcome") || m_settings->getBool("FirstRun")) {
    welcomeDialog = new WelcomeDialog(m_settings, m_profile, this);
    connect(welcomeDialog, SIGNAL(accepted()), this, SLOT(welcomeOk()));
    if (!welcomeDialog->exec())
      mainChannel->displayChoiceServer(true);
  }
  else
    m_clientService->connectToHost();
  
  qsrand(QDateTime(QDateTime::currentDateTime()).toTime_t());
  
  // Пытаемся запустить сервер, в случае неудачи удаляем сервер.
//  daemon = new Server(this);
//  if (daemon->start()) {
//    daemon->setLocalProfile(profile);
//    connect(daemon, SIGNAL(incomingDirectConnection(const QString &, ServerSocket *)), this, SLOT(incomingDirectConnection(const QString &, ServerSocket *)));
//  }
//  else
//    delete daemon;
  
  #ifdef SCHAT_UPDATE
  QTimer::singleShot(0, this, SLOT(update()));
  #endif
}


/*!
 * \brief Обработка события закрытия чата.
 * 
 * \todo Исправить, проблему с препятствием завершению программы обычным путём.
 */
void SChatWindow::closeEvent(QCloseEvent *event)
{
  saveGeometry();
  m_settings->write();

  if (isHidden()) {
    event->accept();
  }
  else {
    if (aboutDialog)
      aboutDialog->hide();
    hide();
    event->ignore();
  }
}


/** [protected]
 * 
 */
bool SChatWindow::event(QEvent *event)
{
  if (event->type() == QEvent::WindowActivate)
    resetTabNotice(m_tabs->currentIndex());

  return QMainWindow::event(event);
}


/** [public slots]
 * 
 */
void SChatWindow::handleMessage(const QString &/*message*/)
{
  showChat();
}


/** [private slots]
 * 
 */
void SChatWindow::about()
{
  if (isHidden())
    show();
  
  if (!aboutDialog) {
    aboutDialog = new AboutDialog(this);
    aboutDialog->show();
  }
   
  aboutDialog->activateWindow();
}


/** [private slots]
 * 
 */
void SChatWindow::accessDenied(quint16 reason)
{
  switch (reason) {
    case ErrorNickAlreadyUse:
      uniqueNick();
      m_clientService->connectToHost();
      break;
      
    case ErrorOldClientProtocol:
      mainChannel->browser->msgOldClientProtocol();
      break;
      
    case ErrorOldServerProtocol:
      mainChannel->browser->msgOldServerProtocol();
      break;
      
    case ErrorBadNickName:
      mainChannel->browser->msgBadNickName(m_profile->nick());
      break;
      
    default:
      mainChannel->browser->msg(tr("<i class='err'>При подключении произошла критическая ошибка с кодом: <b>%1</b></i>").arg(reason));
      break;
  }
  unconnected(false);
}


/** [private slots]
 * Слот вызывается кода `m_clientService` получает пакет с опкодом `OpcodeAccessGranted`,
 * что означает успешное подключение к серверу/сети.
 */
void SChatWindow::accessGranted(const QString &network, const QString &server, quint16 /*level*/)
{
  if (network.isEmpty()) {
    mainChannel->browser->msgReadyForUse(server);
    statusLabel->setText(tr("Успешно подключены к серверу %1").arg(server));
    setWindowTitle(tr("IMPOMEZIA Simple Chat"));
  }
  else {
    mainChannel->browser->msgReadyForUse(network, server);
    statusLabel->setText(tr("Успешно подключены к сети %1 (%2)").arg(network).arg(server));
    setWindowTitle(tr("IMPOMEZIA Simple Chat - %1").arg(network));
  } 
}


/** [private slots]
 * 
 */
void SChatWindow::addTab(const QModelIndex &i)
{
  QStandardItem *item = model.itemFromIndex(i);
  QString nick = item->text();
  if (nick == m_profile->nick())
    return;
  
  int index = tabIndex(nick);
  
  if (index == -1) {
    AbstractProfile profile(item->data(Qt::UserRole + 1).toStringList());
    Tab *tab = new Tab(m_settings, this);
    tab->icon.addFile(":/images/" + profile.gender() + ".png");
    tab->browser->setChannel(nick);
    index = m_tabs->addTab(tab, tab->icon, nick);
    m_tabs->setTabToolTip(index, userToolTip(profile));
  }
  
  m_tabs->setCurrentIndex(index);
}


/** [private slots]
 * 
 */
void SChatWindow::closeChat()
{
  m_clientService->quit();

  saveGeometry();
  m_settings->write();
  qApp->quit();
}


/** [private slots]
 * 
 */
void SChatWindow::closeTab()
{
  int index = m_tabs->currentIndex();
  if (index) {
    QWidget *widget = m_tabs->widget(index);
    m_tabs->removeTab(index);
    widget->deleteLater();
  }
  else
    m_clientService->quit();
}


/** [private slots]
 * Слот вызывается когда m_clientService пытается подключится к серверу.
 * ПАРАМЕТРЫ:
 *   server  -> название сети если network = true, либо адрес сервера.
 *   network -> подключение к сети (true) либо к серверу (false).
 */
void SChatWindow::connecting(const QString &server, bool network)
{
  if (network)
    statusLabel->setText(tr("Идёт подключение к сети %1...").arg(server));
  else
    statusLabel->setText(tr("Идёт подключение к серверу %1...").arg(server));
  
  mainChannel->displayChoiceServer(false);
}


/** [private slots]
 * 
 */
void SChatWindow::fatal()
{
  mainChannel->displayChoiceServer(true);
}


/** [private slots]
 * 
 */
void SChatWindow::genericMessage(const QString &info)
{
  mainChannel->browser->msg(info);
}


/** [private slots]
 * 
 */
void SChatWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::MiddleClick:
      if (isHidden())
        showChat();
      else
        hideChat();
      
    default:
      break;
  }
}


/** [private slots]
 * 
 */
void SChatWindow::linkLeave(quint8 /*numeric*/, const QString &network, const QString &ip)
{
  mainChannel->browser->msg(tr("<i class='gr'>Сервер <b>%1</b> отключился от сети <b>%2</b></i>").arg(ip).arg(network));
}


/** [private slots]
 * 
 */
void SChatWindow::message(const QString &sender, const QString &message)
{
  if ((m_tabs->currentIndex() != 0) || (!isActiveWindow()))
    startNotice(0);
  
  mainChannel->msgNewMessage(sender, message);  
}


/** [private slots]
 * 
 */
void SChatWindow::messageClicked()
{
  QProcess::startDetached('"' + qApp->applicationDirPath() + "/schat.exe\"");
  
  closeChat();
}


/** [private slots]
 * 
 */
void SChatWindow::newLink(quint8 /*numeric*/, const QString &network, const QString &ip)
{
  mainChannel->browser->msg(tr("<i class='gr'>Сервер <b>%1</b> подключился к сети <b>%2</b></i>").arg(ip).arg(network));
}


/** [private slots]
 * 
 */
void SChatWindow::newNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name)
{
  QStandardItem *item = findItem(nick);
  
  if (item) {
    item->setText(newNick);
    model.sort(0);
    
    int index = tabIndex(nick);
    if (index != -1) {
      m_tabs->setTabText(index, newNick);
      AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->widget(index));
      tab->browser->setChannel(newNick);
      tab->browser->msgChangedNick(gender, nick, newNick);
    }
    
    newProfile(gender, newNick, name);
    
    mainChannel->browser->msgChangedNick(gender, nick, newNick);
  }  
}


/*!
 * \brief Обработка изменения профиля пользователя.
 * 
 * При наличии пользователя в списке функция обновляет информацию о пользователе,
 * а также если имеются открытые приваты, то вкладка также обновляется.
 * 
 * \param gender Новый пол пользователя.
 * \param nick Ник пользователя.
 * \param name Новое полное имя.
 */
void SChatWindow::newProfile(quint8 gender, const QString &nick, const QString &name)
{
  QStandardItem *item = findItem(nick);

  if (item) {    
    AbstractProfile profile(item->data(Qt::UserRole + 1).toStringList());
    profile.setGender(gender);
    profile.setNick(nick);
    profile.setFullName(name);
    item->setIcon(QIcon(":/images/" + profile.gender() + ".png"));
    item->setToolTip(userToolTip(profile));
    item->setData(profile.pack(), Qt::UserRole + 1);

    int index = tabIndex(nick);
    if (index != -1) {
      m_tabs->setTabToolTip(index, userToolTip(profile));
      AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->widget(index));
      tab->icon.addFile(":/images/" + profile.gender() + ".png");

      if (!tab->notice)
        m_tabs->setTabIcon(index, tab->icon);
    }
  }
}


/** [private slots]
 * 
 */
void SChatWindow::newUser(const QStringList &list, quint8 echo, quint8 /*numeric*/)
{
#ifdef SCHAT_DEBUG
  qDebug() << "SChatWindow::newUser(const QStringList &, quint8, quint8)" << echo ;
#endif

  AbstractProfile profile(list);
  QString nick = profile.nick();
  
  if (nick == m_profile->nick())
    if (findItem(profile.nick()))
      return;
  
  QStandardItem *item = new QStandardItem(QIcon(":/images/" + profile.gender() + ".png"), nick);
  item->setData(profile.pack(), Qt::UserRole + 1);
  item->setToolTip(userToolTip(profile));

  // Свой ник выделяем жирным шрифтом
  if (nick == m_profile->nick()) {
    QFont font;
    font.setBold(true);
    item->setFont(font);
  }

  model.appendRow(item);
  model.sort(0);
  
  if (m_profile->nick() == nick)
    echo = 0;

  // Если включено эхо, добавляем в основной канал и приваты, сообщение о новом участнике.
  if (echo == 1) {
    int index = tabIndex(nick);
    if (index != -1) {
      AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->widget(index));
      if (tab->type == AbstractTab::Private)
        tab->browser->msgNewParticipant(profile.genderNum(), nick);
    }
    
    mainChannel->browser->msgNewParticipant(profile.genderNum(), nick);
  }
}


/** [private slots]
 * 
 */
void SChatWindow::notice()
{
  if (currentTrayIcon) {
    trayIcon->setIcon(QIcon(":/images/notice.png"));
    currentTrayIcon = false;
  }
  else {
    trayIcon->setIcon(QIcon(":/images/logo16.png"));
    currentTrayIcon = true;
  }
}


/** [private slots]
 * 
 */
void SChatWindow::privateMessage(quint8 flag, const QString &nick, const QString &message)
{
#ifdef SCHAT_DEBUG
  qDebug() << "SChatWindow::privateMessage()" << flag << nick << message;
#endif

  QStandardItem *item = findItem(nick);
  if (!item)
    return;
  
  Tab *tab = 0;
  int index = tabIndex(nick);
  
  if (index == -1) {
    AbstractProfile profile(item->data(Qt::UserRole + 1).toStringList());
    tab = new Tab(m_settings, this);
    tab->icon.addFile(":/images/" + profile.gender() + ".png");
    tab->browser->setChannel(nick);
    index = m_tabs->addTab(tab, tab->icon, nick);
    m_tabs->setTabToolTip(index, userToolTip(profile));
    m_tabs->setCurrentIndex(index);
  }
  else
    tab = qobject_cast<Tab *>(m_tabs->widget(index));

  if (tab)
    if (flag == 1)
      tab->browser->msgNewMessage(m_profile->nick(), message);
    else
      tab->browser->msgNewMessage(nick, message);
  
  if ((m_tabs->currentIndex() != index) || (!isActiveWindow()))
    startNotice(index);
}


/** [private slots]
 * 
 */
void SChatWindow::resetTabNotice(int index)
{
  if (index == -1)
    return;
    
  AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->widget(index));
  if (tab->notice) {
    m_tabs->setTabIcon(index, tab->icon);
    tab->notice = false;
  }
  
  int count = m_tabs->count();
  for (int i = 0; i < count; ++i) {
    AbstractTab *t = static_cast<AbstractTab *>(m_tabs->widget(i));
    if (t->notice)
      return;
  }
  
  if (noticeTimer->isActive()) {
    noticeTimer->stop();
    currentTrayIcon = true;
    trayIcon->setIcon(QIcon(":/images/logo16.png"));
  }
}


/*!
 * \brief Обработка отправки сообщения.
 */
void SChatWindow::sendMsg(const QString &message)
{
  AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->currentWidget());

  if (parseCmd(tab, message))
    return;

  if (m_clientService) {
    QString channel;
    m_tabs->currentIndex() == 0 ? channel = "" : channel = m_tabs->tabText(m_tabs->currentIndex());
    if (m_clientService->sendMessage(channel, message));
      m_send->clear();
  }
}


/*!
 * \brief Показ сообщения от сервера.
 * 
 * \todo Сделать показ сообщения в активной вкладке.
 */
void SChatWindow::serverMessage(const QString &msg)
{
  mainChannel->browser->msg(msg);
}


/** [private slots]
 * 
 */
#ifdef SCHAT_UPDATE
void SChatWindow::update()
{
  if (!m_updateTimer->isActive())
    m_updateTimer->start();
  
  if (!m_updateNotify) {
    m_updateNotify = new UpdateNotify(m_settings, this);
    connect(m_updateNotify, SIGNAL(done(int)), SLOT(updateGetDone(int)));
  }
  
  m_updateNotify->execute();
}
#endif


/** [private slots]
 * 
 */
void SChatWindow::settingsChanged(int notify)
{
  switch (notify) {
    case Settings::NetworkSettingsChanged:
    case Settings::ServerChanged:
      m_clientService->connectToHost();
      break;
    
    case Settings::ProfileSettingsChanged:
      m_clientService->sendNewProfile();
      break;
      
    case Settings::ByeMsgChanged:
      m_clientService->sendByeMsg();
      break;

    #ifdef SCHAT_UPDATE
    case Settings::UpdateSettingsChanged:
      m_updateTimer->setInterval(m_settings->getInt("Updates/CheckInterval") * 60 * 1000);
      break;
    #endif
  
    default:
      break;
  }
}


/** [private slots]
 * Слот вызывается когда в `m_clientService` нет активного подключения.
 */
void SChatWindow::unconnected(bool echo)
{
  statusLabel->setText(tr("Не подключено"));
  model.clear();
  
  if (echo)
    mainChannel->browser->msgDisconnect();
}


/** [private slots]
 * Выход удалённого пользователя из чата, опкод `OpcodeUserLeave`.
 */
void SChatWindow::userLeave(const QString &nick, const QString &bye, quint8 flag)
{
  QStandardItem *item = findItem(nick);
  
  if (item) {
    QStringList list = item->data(Qt::UserRole + 1).toStringList();
    model.removeRow(model.indexFromItem(item).row());
    
    if (flag == 1) {
      AbstractProfile profile(list);
      int index = tabIndex(nick);
      if (index != -1) {
        AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->widget(index));
        if (tab->type == AbstractTab::Private)
          tab->browser->msgParticipantLeft(profile.genderNum(), nick, bye);
      }
    
      mainChannel->browser->msgParticipantLeft(profile.genderNum(), nick, bye);
    }
  }  
}


/** [private slots]
 * Слот вызывается при завершении работы программы обновления
 */
#ifdef SCHAT_UPDATE
void SChatWindow::updateGetDone(int code)
{
  if (!m_updateNotify)
    m_updateNotify->deleteLater();
  
  QSettings s(qApp->applicationDirPath() + "/schat.conf", QSettings::IniFormat, this);
  
  if (code == 0) {
    QString version = s.value("Updates/LastDownloadedCoreVersion", "").toString(); 
    trayIcon->showMessage(tr("Доступно обновление до версии %1").arg(version), tr("Щёлкните здесь для того чтобы установить это обновление прямо сейчас."), QSystemTrayIcon::Information, 60000);
  }
  else
    s.setValue("Updates/ReadyToInstall", false);
}
#endif


/** [private slots]
 * 
 */
void SChatWindow::settingsPage(int page)
{
  if (isHidden())
    show();
  
  if (!settingsDialog) {
    settingsDialog = new SettingsDialog(m_profile, m_settings, this);
    settingsDialog->show();
  }
  
  settingsDialog->setPage(page);
  settingsDialog->activateWindow();
}


/** [private slots]
 * 
 */
void SChatWindow::welcomeOk()
{
  welcomeDialog->deleteLater();
  
  m_clientService->connectToHost();
}


/** [private]
 * 
 */
bool SChatWindow::parseCmd(AbstractTab *tab, const QString &message)
{
  QString text = ChannelLog::toPlainText(message);

  // команда /bye
  if (text.startsWith("/bye", Qt::CaseInsensitive)) {   
    if (text.startsWith("/bye ", Qt::CaseInsensitive))
      m_clientService->sendByeMsg(text.mid(text.indexOf(QChar(' '))));

    m_clientService->quit();
  }
  // команда /exit
  else if (text.startsWith("/exit", Qt::CaseInsensitive)) {
    closeChat();
  }
  // команда /help
  else if (text.startsWith("/help", Qt::CaseInsensitive)) {
    tab->browser->msg(tr(
        "<b class='info'>Доступные команды:</b><br />"
        "<b>/bye [текст сообщения]</b><span class='info'> - Отключится от сервера/сети, опционально можно указать альтернативное сообщение о выходе.</span><br />"
        "<b>/exit</b><span class='info'> - Выход из чата.</span><br />"
        "<b>/help</b><span class='info'> - Отображает подсказу о командах.</span><br />"
        "<b>/log</b><span class='info'> - Открывает папку с файлами журнала чата.</span><br />"
        "<b>/me &lt;текст сообщения&gt;</b><span class='info'> - Отправка сообщения о себе от третьего лица, например о том что вы сейчас делаете.</span><br />"
        "<b>/nick &lt;новый ник&gt;</b><span class='info'> - Позволяет указать новый ник, если указанный ник уже занят, произойдёт автоматическое отключение.</span><br />"
        "<b>/server info</b><span class='info'> - Просмотр информации о сервере</span>"
        ));
  }
  // команда /log
  else if (text.startsWith("/log", Qt::CaseInsensitive)) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(qApp->applicationDirPath() + "/log"));
  }
  // команда /nick
  else if (text.startsWith("/nick ", Qt::CaseInsensitive)) {
    QString newNick = text.mid(text.indexOf(QChar(' ')));
    if (AbstractProfile::isValidNick(newNick) && m_profile->nick() != newNick) {
      m_profile->setNick(newNick);
      m_clientService->sendNewProfile();
    }
  }
  else
    return false;

  m_send->clear();
  return true;
}


/** [private]
 * 
 */
int SChatWindow::tabIndex(const QString &s, int start) const
{
  int count = m_tabs->count();
  int tab = -1;
  
  if (count > start)
    for (int i = start; i <= count; ++i)
      if (m_tabs->tabText(i) == s) {
        tab = i;
        break;
      }
  
  return tab;
}


/** [private]
 * 
 */
//Profile* SChatWindow::profileFromItem(const QStandardItem *item)
//{
//  return new Profile(quint8(item->data(Qt::UserRole + 1).toUInt()), item->data(Qt::UserRole + 2).toStringList(), this);
//}


/** [private]
 * 
 */
QStandardItem* SChatWindow::findItem(const QString &nick) const
{
  QList<QStandardItem *> items;
  
  items = model.findItems(nick, Qt::MatchCaseSensitive);
  if (items.size() == 1)
    return items[0];
  else
    return 0;  
}


/** [private] static
 * 
 */
QString SChatWindow::userToolTip(const AbstractProfile &profile)
{
  QString p_agent = profile.userAgent();
  p_agent.replace(QChar('/'), QChar(' '));
  QString p_name;
  profile.fullName().isEmpty() ? p_name = tr("&lt;не указано&gt;") : p_name = profile.fullName();
  
  return tr("<h3><img src='%1' align='left'> %2</h3>"
            "<table><tr><td>Настоящее имя:</td><td>%3</td></tr>"
            "<tr><td>Клиент:</td><td>%4</td></tr>"
            "<tr><td>IP-адрес:</td><td>%5</td></tr></table>")
            .arg(":/images/" + profile.gender() + ".png").arg(profile.nick()).arg(p_name).arg(p_agent).arg(profile.host());
}


/** [private]
 * 
 */
void SChatWindow::createActions()
{
  // О Программе...
  aboutAction = new QAction(QIcon(":/images/logo16.png"), tr("О Программе..."), this);
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
  
  // Открытие новой вкладки, для создания нового подключения
//  addTabAction = new QAction(QIcon(":/images/tab_new.png"), tr("Новое прямое подключение"), this);
//  addTabAction->setShortcut(tr("Ctrl+N"));
//  addTabAction->setStatusTip(tr("Открытие новой вкладки, для создания нового прямого подключения"));
//  connect(addTabAction, SIGNAL(triggered()), this, SLOT(addTab()));
  
  // Разорвать текущее соединение
  closeTabAction = new QAction(QIcon(":/images/tab_close.png"), tr("Разорвать текущее соединение"), this);
  closeTabAction->setStatusTip(tr("Разорвать текущее соединение"));
  connect(closeTabAction, SIGNAL(triggered()), this, SLOT(closeTab()));
  
  // Интерфейс...
  interfaceSetAction = new QAction(QIcon(":/images/appearance.png"), tr("Интерфейс..."), this);
  connect(interfaceSetAction, SIGNAL(triggered()), this, SLOT(settingsInterface()));
  
  // Сеть...
  networkSetAction = new QAction(QIcon(":/images/network.png"), tr("Сеть..."), this);
  connect(networkSetAction, SIGNAL(triggered()), this, SLOT(settingsNetwork()));
  
  // Личные данные...
  profileSetAction = new QAction(QIcon(":/images/profile.png"), tr("Личные данные..."), this);
  profileSetAction->setShortcut(tr("Ctrl+F12"));
  connect(profileSetAction, SIGNAL(triggered()), this, SLOT(settingsProfile()));
  
  // Обновления...
  #ifdef SCHAT_UPDATE
  updateSetAction = new QAction(QIcon(":/images/update.png"), tr("Обновления..."), this);
  connect(updateSetAction, SIGNAL(triggered()), this, SLOT(settingsUpdate()));
  #endif
  
  // Выход из программы
  quitAction = new QAction(QIcon(":/images/quit.png"), tr("&Выход"), this);
  connect(quitAction, SIGNAL(triggered()), this, SLOT(closeChat()));
}


/** [private]
 * 
 */
void SChatWindow::createCornerWidgets()
{
//  QToolButton *addTabButton = new QToolButton(this);
//  addTabButton->setDefaultAction(addTabAction);
//  addTabButton->setAutoRaise(true);
//  tabWidget->setCornerWidget(addTabButton, Qt::TopLeftCorner);
  
  QToolButton *closeTabButton = new QToolButton(this);
  closeTabButton->setDefaultAction(closeTabAction);
  closeTabButton->setAutoRaise(true);
  m_tabs->setCornerWidget(closeTabButton, Qt::TopRightCorner);  
}


/** [private]
 * 
 */
void SChatWindow::createService()
{
  m_clientService = new ClientService(m_profile, &m_settings->network, this);
  connect(m_clientService, SIGNAL(connecting(const QString &, bool)), SLOT(connecting(const QString &, bool)));
  connect(m_clientService, SIGNAL(unconnected(bool)), SLOT(unconnected(bool)));
  connect(m_clientService, SIGNAL(newUser(const QStringList &, quint8, quint8)), SLOT(newUser(const QStringList &, quint8, quint8)));
  connect(m_clientService, SIGNAL(accessGranted(const QString &, const QString &, quint16)), SLOT(accessGranted(const QString &, const QString &, quint16)));
  connect(m_clientService, SIGNAL(userLeave(const QString &, const QString &, quint8)), SLOT(userLeave(const QString &, const QString &, quint8)));
  connect(m_clientService, SIGNAL(accessDenied(quint16)), SLOT(accessDenied(quint16)));
  connect(m_clientService, SIGNAL(message(const QString &, const QString &)), SLOT(message(const QString &, const QString &)));
  connect(m_clientService, SIGNAL(privateMessage(quint8, const QString &, const QString &)), SLOT(privateMessage(quint8, const QString &, const QString &)));
  connect(m_clientService, SIGNAL(fatal()), SLOT(fatal()));
  connect(m_clientService, SIGNAL(serverMessage(const QString &)), SLOT(serverMessage(const QString &)));
  connect(m_clientService, SIGNAL(newNick(quint8, const QString &, const QString &, const QString &)), SLOT(newNick(quint8, const QString &, const QString &, const QString &)));
  connect(m_clientService, SIGNAL(newProfile(quint8, const QString &, const QString &)), SLOT(newProfile(quint8, const QString &, const QString &)));
  connect(m_clientService, SIGNAL(newLink(quint8, const QString &, const QString &)), SLOT(newLink(quint8, const QString &, const QString &)));
  connect(m_clientService, SIGNAL(linkLeave(quint8, const QString &, const QString &)), SLOT(linkLeave(quint8, const QString &, const QString &)));
}


/** [private]
 * 
 */
void SChatWindow::createToolButtons()
{
  QMenu *iconMenu = new QMenu(this);
  iconMenu->addAction(profileSetAction);
  iconMenu->addAction(networkSetAction);
  iconMenu->addAction(interfaceSetAction);
  
  #ifdef SCHAT_UPDATE
  iconMenu->addAction(updateSetAction);
  #endif
  
  // Настройка
  m_settingsButton = new QToolButton(this);
  m_settingsButton->setIcon(QIcon(":/images/settings.png"));
  m_settingsButton->setToolTip(tr("Настройка..."));
  m_settingsButton->setAutoRaise(true);
  m_settingsButton->setMenu(iconMenu);
  m_settingsButton->setPopupMode(QToolButton::InstantPopup);
  
  QToolButton *aboutButton = new QToolButton(this);
  aboutButton->setDefaultAction(aboutAction);
  aboutButton->setAutoRaise(true);
  
  QFrame *line = new QFrame(this);
  line->setFrameShape(QFrame::VLine);
  line->setFrameShadow(QFrame::Sunken);
  
  toolsLayout->addWidget(line);
  toolsLayout->addWidget(m_settingsButton);
  toolsLayout->addWidget(aboutButton);
  toolsLayout->addStretch();
  toolsLayout->setSpacing(0);  
}


/** [private]
 * 
 */
void SChatWindow::createTrayIcon()
{
  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(aboutAction);
  trayIconMenu->addAction(profileSetAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(quitAction);
  
  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setIcon(QIcon(":/images/logo16.png"));
  trayIcon->setToolTip(tr("IMPOMEZIA Simple Chat %1").arg(SCHAT_VERSION));
  trayIcon->setContextMenu(trayIconMenu);
  trayIcon->show();
  currentTrayIcon = true;
}


/** [private]
 * 
 */
void SChatWindow::hideChat()
{
  if (settingsDialog)
    settingsDialog->hide();
          
  if (aboutDialog)
    aboutDialog->hide();
        
  hide();
}


/*!
 * \brief Восстанавливает геометрию окна.
 */
void SChatWindow::restoreGeometry()
{
  resize(m_settings->size());
  QPoint pos = m_settings->pos();
  if (pos.x() != -999 && pos.y() != -999)
    move(pos);

  splitter->restoreState(m_settings->splitter());
}


/*!
 * \brief Сохраняет еометрию окна.
 */
void SChatWindow::saveGeometry()
{
  m_settings->setPos(pos());
  m_settings->setSize(size());
  m_settings->setSplitter(splitter->saveState());
}


/** [private]
 * 
 */
void SChatWindow::showChat()
{
  show();
  activateWindow();
  
  if (aboutDialog)
    aboutDialog->show();
  
  if (settingsDialog)
    settingsDialog->show();
}


/** [private]
 * 
 */
void SChatWindow::startNotice(int index)
{
  if (index == -1)
    return;
  
  AbstractTab *tab = static_cast<AbstractTab *>(m_tabs->widget(index));
  tab->notice = true;
  m_tabs->setTabIcon(index, QIcon(":/images/notice.png"));
  if (!noticeTimer->isActive()) {
    currentTrayIcon = false;
    trayIcon->setIcon(QIcon(":/images/notice.png"));
    noticeTimer->start();
  }
}


/** [private]
 * 
 */
void SChatWindow::uniqueNick()
{
  m_profile->setNick(m_profile->nick() + QString().setNum(qrand() % 99));
}
