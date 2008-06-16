/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>
#include <QtNetwork>

#include "aboutdialog.h"
#include "directchannel.h"
#include "directchannelserver.h"
#include "mainchannel.h"
#include "profile.h"
#include "protocol.h"
#include "schatwindow.h"
#include "settings.h"
#include "settingsdialog.h"
#include "tab.h"
#include "version.h"
#include "welcomedialog.h"

static const int reconnectTimeout = 3 * 1000;


/** [public]
 * 
 */
SChatWindow::SChatWindow(QWidget *parent)
  : QMainWindow(parent)
{
  centralWidget = new QWidget(this);  
  splitter      = new QSplitter(centralWidget);
  lineEdit      = new QLineEdit(centralWidget);
  tabWidget     = new QTabWidget(splitter);
  rightWidget   = new QWidget(splitter);
  listView      = new QListView(rightWidget);  
  rightLayout   = new QVBoxLayout(rightWidget);
  mainLayout    = new QVBoxLayout(centralWidget);
  sendLayout    = new QHBoxLayout;
  toolsLayout   = new QHBoxLayout;
  statusbar     = new QStatusBar(this);
  sendButton    = new QToolButton(centralWidget);
  statusLabel   = new QLabel(this);
  profile       = new Profile(this);
  settings      = new Settings(profile, this);
  noticeTimer   = new QTimer(this);
  noticeTimer->setInterval(800);
  m_updateTimer = new QTimer(this);
  m_updateTimer->setInterval(60 * 1000);
  
  m_reconnectTimer = new QTimer(this);
  m_reconnectTimer->setInterval(reconnectTimeout);
 
  state = Disconnected;
  
  splitter->addWidget(tabWidget);
  splitter->addWidget(rightWidget);
  splitter->setStretchFactor(0, 4);
  splitter->setStretchFactor(1, 1);
  
  rightLayout->addLayout(toolsLayout);
  rightLayout->addWidget(listView);
  rightLayout->setMargin(0);
  rightLayout->setSpacing(4);
  
  sendLayout->addWidget(lineEdit);
  sendLayout->addWidget(sendButton);
  
  mainLayout->addWidget(splitter);
  mainLayout->addLayout(sendLayout);
  mainLayout->setMargin(4);
  mainLayout->setSpacing(3);
  
  setCentralWidget(centralWidget);
  setStatusBar(statusbar);
  statusbar->addWidget(statusLabel, 1);
  statusLabel->setText(tr("Не подключено"));
  
  setWindowTitle(tr("IMPOMEZIA Simple Chat"));
  
  tabWidget->setElideMode(Qt::ElideRight);
  listView->setFocusPolicy(Qt::NoFocus);
  listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  listView->setModel(&model);
  settings->read();
  
  createActions();
  createCornerWidgets();
  createToolButtons();
  createTrayIcon();
  
  connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
  connect(listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addTab(const QModelIndex &)));
  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
  connect(noticeTimer, SIGNAL(timeout()), this, SLOT(notice()));
  connect(m_reconnectTimer, SIGNAL(timeout()), this, SLOT(newConnection()));
  connect(m_updateTimer, SIGNAL(timeout()), SLOT(update()));
  connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(resetTabNotice(int)));
  connect(trayIcon, SIGNAL(messageClicked()), SLOT(messageClicked()));
  
  connect(settings, SIGNAL(networkSettingsChanged()), this, SLOT(networkSettingsChanged()));
  connect(settings, SIGNAL(profileSettingsChanged()), this, SLOT(profileSettingsChanged()));
  connect(settings, SIGNAL(serverChanged()), this, SLOT(newConnection()));
  
  mainChannel = new MainChannel(settings, this);
  mainChannel->icon.addFile(":/images/main.png");
  tabWidget->setCurrentIndex(tabWidget->addTab(mainChannel, tr("Общий")));
  tabWidget->setTabIcon(0, mainChannel->icon);
  
  if (!settings->hideWelcome || settings->firstRun) {
    welcomeDialog = new WelcomeDialog(settings, profile, this);
    connect(welcomeDialog, SIGNAL(accepted()), this, SLOT(welcomeOk()));
    if (!welcomeDialog->exec())
      mainChannel->displayChoiceServer(true);
  }
  else
    newConnection();
  
  qsrand(QDateTime(QDateTime::currentDateTime()).toTime_t());
  
  // Пытаемся запустить сервер, в случае неудачи удаляем сервер.
  daemon = new Server(this);
  if (daemon->start()) {
    daemon->setLocalProfile(profile);
    connect(daemon, SIGNAL(incomingDirectConnection(const QString &, ServerSocket *)), this, SLOT(incomingDirectConnection(const QString &, ServerSocket *)));
  }
  else
    delete daemon;
  
  QTimer::singleShot(0, this, SLOT(update()));
}


/** [public]
 * 
 */
void SChatWindow::reconnect()
{
  if (state == Connected) {
    mainChannel->browser->msgDisconnect();
    mainChannel->browser->add(tr("<div class='nb'>(%1) <i class='info'>Пытаемся подключится к сети с новыми настройками</i></div>").arg(ChatBrowser::currentTime()));
    state = WaitingForConnected;
    clientSocket->quit();
  }  
}


/** [protected]
 * 
 */
void SChatWindow::closeEvent(QCloseEvent *event)
{ // TODO Разобраться с корректным завершением программы.
  settings->write();
  
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
    resetTabNotice(tabWidget->currentIndex());

  return QMainWindow::event(event);
}


/** [public slots]
 * 
 */
void SChatWindow::incomingDirectConnection(const QString &n, ServerSocket *socket)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "SChatWindow::incomingDirectConnection(const QString &n, ServerSocket *socket)" << n;
  #endif
  
  int index = tabIndex(QChar('#') + n);
  
  if (index == -1) {
    Profile *p = new Profile(socket->sex(), socket->participantInfo(), this);
    DirectChannelServer *ch = new DirectChannelServer(profile, socket, this);
    ch->icon.addFile(Profile::sexIconString(socket->sex()));
    index = tabWidget->addTab(ch, ch->icon, QChar('#') + n);
    tabWidget->setTabToolTip(index, p->toolTip());
    p->deleteLater();    
  }
  else if (DirectChannelServer *channel  = qobject_cast<DirectChannelServer *>(tabWidget->widget(index)))
    channel->changeSocket(socket);  
    
  tabWidget->setCurrentIndex(index);
}


/** [public slots]
 * Уведомление от классов `DirectChannel` и `DirectChannelServer` о новом сообщении
 * Слот при необходимости запускает механизм уведомления о новом сообщении.
 */
void SChatWindow::newDirectMessage()
{
  AbstractTab *channel = static_cast<AbstractTab *>(sender());
  int index = tabWidget->indexOf(channel);
  
  if ((tabWidget->currentIndex() != index) || (!isActiveWindow()))
    startNotice(index);
}


/** [public slots]
 * Слот вызывается из класса `DirectChannel` когда инкапсулированный в него
 * `ClientSocket` отправляет сигнал `newParticipant()`.
 * Слот переименовывает вкладку в соответствии с ником ('#' + ник),
 * устанавливает всплывающую подсказку и устанавливает иконку.
 * *
 * quint16 sex             - пол участника.
 * const QStringList &info - унифицированный список для создания профиля.
 */
void SChatWindow::newDirectParticipant(quint16 sex, const QStringList &info)
{
  AbstractTab *channel = static_cast<AbstractTab *>(sender());
  Profile *p = new Profile(sex, info, this);
  int index = tabWidget->indexOf(channel);
  channel->icon.addFile(Profile::sexIconString(sex));
  tabWidget->setTabText(index, QChar('#') + info.at(0));
  tabWidget->setTabToolTip(index, p->toolTip());
  tabWidget->setTabIcon(index, channel->icon);
  p->deleteLater();  
}


/** [public slots]
 * 
 */
void SChatWindow::newMessage(const QString &/*nick*/, const QString &/*message*/)
{
  if ((tabWidget->currentIndex() != 0) || (!isActiveWindow()))
    startNotice(0);
}


/** [public slots]
 * 
 */
void SChatWindow::newParticipant(quint16 sex, const QStringList &info, bool echo)
{
  QStandardItem *item = new QStandardItem(QIcon(Profile::sexIconString(sex)), info.at(0));
  Profile *p = new Profile(sex, info, this);
  item->setData(sex, Qt::UserRole + 1);
  item->setData(info, Qt::UserRole + 2);
  item->setToolTip(p->toolTip());  
  p->deleteLater();
  
  // Свой ник выделяем жирным шрифтом
  if (info.at(0) == profile->nick()) {
    QFont font;
    font.setBold(true);
    item->setFont(font);
  }
  
  model.appendRow(item);
  model.sort(0);
  
  // Если включено эхо, добавляем в основной канал и приваты, сообщение о новом участнике.
  if (echo) {
    int index = tabIndex(info.at(0));
    if (index != -1) {
      AbstractTab *tab = static_cast<AbstractTab *>(tabWidget->widget(index));
      if (tab->type == AbstractTab::Private)
        tab->browser->msgNewParticipant(sex, info.at(0));
    }
    
    mainChannel->browser->msgNewParticipant(sex, info.at(0));
  }
}


/** [public slots]
 * 
 */
void SChatWindow::newPrivateMessage(const QString &nick, const QString &message, const QString &sender)
{
  int index = tabIndex(nick);
  QStandardItem *item = findItem(nick);
  Tab *tab = 0;
  
  if (index == -1 ) {
    if (item) {
      Profile *p = profileFromItem(item);
      tab = new Tab(this);
      tab->icon.addFile(Profile::sexIconString(profile->sex()));
      index = tabWidget->addTab(tab, tab->icon, nick);
      tabWidget->setCurrentIndex(index);
      tabWidget->setTabToolTip(index, p->toolTip());
      p->deleteLater();
    }
  }
  else 
    tab = qobject_cast<Tab *>(tabWidget->widget(index));
  
  if (tab)
    tab->browser->msgNewMessage(sender, message);
  
  if ((tabWidget->currentIndex() != index) || (!isActiveWindow()))
    startNotice(index);
}


/** [public slots]
 * 
 */
void SChatWindow::participantLeft(const QString &nick)
{
  quint8 sex = 0;
  
  QList<QStandardItem *> items;
  items = model.findItems(nick, Qt::MatchFixedString);
  if (items.size() == 1) {
    sex = quint8(items[0]->data(Qt::UserRole + 1).toUInt());
    QModelIndex index = model.indexFromItem(items[0]);
    model.removeRow(index.row());
  }
  
  int index = tabIndex(nick);
  if (index != -1) {
    AbstractTab *tab = static_cast<AbstractTab *>(tabWidget->widget(index));
    if (tab->type == AbstractTab::Private)
      tab->browser->msgParticipantLeft(sex, nick);
  }
  
  mainChannel->browser->msgParticipantLeft(sex, nick);
}


/** [public slots]
 * 
 */
void SChatWindow::readyForUse()
{
  if (m_reconnectTimer->isActive())
    m_reconnectTimer->stop();
  
  state = Connected;
  QString peerAddress = clientSocket->peerAddress().toString();
  
  // Различные сообщения при подключении к сети и одиночному серверу
  if (settings->network.isNetwork()) {
    QString networkName = settings->network.name();
    mainChannel->browser->msgReadyForUse(networkName, peerAddress);
    statusLabel->setText(tr("Успешно подключены к сети %1 (%2)").arg(networkName).arg(peerAddress));
    setWindowTitle(tr("Simple Chat - %1").arg(networkName));
  }
  else {
    mainChannel->browser->msgReadyForUse(peerAddress);
    statusLabel->setText(tr("Успешно подключены к %1").arg(peerAddress));
    setWindowTitle(tr("Simple Chat"));
  }
  
  mainChannel->displayChoiceServer(false);
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
void SChatWindow::addTab()
{
  QString label = tr("Новое подключение");
  int index = tabIndex(label);
  
  if (index > 0)
    tabWidget->setCurrentIndex(index);
  else
    tabWidget->setCurrentIndex(tabWidget->addTab(new DirectChannel(profile, this), QIcon(":/images/new.png"), label));  
}


/** [private slots]
 * 
 */
void SChatWindow::addTab(const QModelIndex &i)
{
  QStandardItem *item = model.itemFromIndex(i);
  QString nick        = item->text();
  int index           = tabIndex(nick);
  
  if (index == -1) {
    Profile *p = profileFromItem(item);
    Tab *tab = new Tab(this);
    tab->icon.addFile(Profile::sexIconString(p->sex()));
    index = tabWidget->addTab(tab, tab->icon, nick);
    tabWidget->setTabToolTip(index, p->toolTip());
    p->deleteLater();
  }
  
  tabWidget->setCurrentIndex(index);
}


/** [private slots]
 * 
 */
void SChatWindow::changedNick(quint16 sex, const QStringList &list)
{
  QString oldNick = list.at(0);
  QString nick = list.at(1);
  QString name = list.at(2);
  QStandardItem *item = findItem(oldNick);
  
  if (item) {
    item->setText(nick);
    model.sort(0);
    
    int index = tabIndex(oldNick);
    if (index != -1) {
      tabWidget->setTabText(index, nick);
      AbstractTab *tab = static_cast<AbstractTab *>(tabWidget->widget(index));
      tab->browser->msgChangedNick(sex, oldNick, nick);
    }
    
    changedProfile(sex, QStringList() << nick << name, false);
    
    mainChannel->browser->msgChangedNick(sex, oldNick, nick);
  }  
}


/** [private slots]
 * 
 */
void SChatWindow::changedProfile(quint16 sex, const QStringList &list, bool echo)
{
  Profile *p = 0;
  QString nick = list.at(0);
  QString name = list.at(1);
  QStandardItem *item = findItem(nick);
  
  if (item) {
    p = profileFromItem(item);
    p->setSex(sex);
    p->setNick(nick);
    p->setFullName(name);
    item->setIcon(QIcon(Profile::sexIconString(sex)));
    item->setToolTip(p->toolTip());
    item->setData(sex, Qt::UserRole + 1);
    item->setData(p->toList(), Qt::UserRole + 2);
    
    int index = tabIndex(nick);
    if (index != -1) {
      tabWidget->setTabToolTip(index, p->toolTip());
      
      AbstractTab *tab = static_cast<AbstractTab *>(tabWidget->widget(index));
      tab->icon.addFile(Profile::sexIconString(sex));
      
      if (!tab->notice)
        tabWidget->setTabIcon(index, tab->icon);
      
      if (echo)
        tab->browser->msgChangedProfile(sex, nick);
    }
    
    if (echo)
      mainChannel->browser->msgChangedProfile(sex, nick);
  }
}


/** [private slots]
 * 
 */
void SChatWindow::closeChat()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "SChatWindow::closeChat()";
  #endif
  
  if (clientSocket)
    clientSocket->quit();
  
  settings->write();
  qApp->quit();
}


/** [private slots]
 * 
 */
void SChatWindow::closeTab()
{
  int index = tabWidget->currentIndex();
  if (index) {
    QWidget *widget = tabWidget->widget(index);
    tabWidget->removeTab(index);
    Tab *tab = static_cast<Tab *>(widget);
    tab->deleteLater();
  }
  else {
    if (state == Connected) {
      state = Stopped;
      clientSocket->quit();
    }
  }
}


/** [private slots]
 * 
 */
void SChatWindow::connectionError(QAbstractSocket::SocketError /* socketError */)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "DirectChannel::connectionError(QAbstractSocket::SocketError /* socketError */)";
  qDebug() << "SOCKET ERROR:" << clientSocket->errorString();
  #endif
  
  removeConnection();
}


/** [private slots]
 * 
 */
void SChatWindow::disconnected()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "SChatWindow::disconnected()";
  #endif
  
  removeConnection();
}


/** [private slots]
 * 
 */
void SChatWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::MiddleClick:
      if (isHidden()) {
        show();
        activateWindow();
        if (aboutDialog)
          aboutDialog->show();
      }
      else {
        if (aboutDialog)
          aboutDialog->hide();
        hide();
      }
      
    default:
      break;
  }
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
 * Слот вызывается при изменении сетевых настроек.
 * Инициаторы:
 *   void NetworkSettings::save() (через `Settings::notify(int)`)
 */
void SChatWindow::networkSettingsChanged()
{
  if (state == Connected) {
    mainChannel->browser->msgDisconnect();
    mainChannel->browser->add(tr("<div class='nb'>(%1) <i class='info'>Изменены настройки сети, пытаемся подключится...</i></div>").arg(ChatBrowser::currentTime()));
  }
  
  newConnection();
}


/** [private slots]
 * Инициаторы:
 *   MainChannel::serverChanged() (через `Settings::notify(int)`)
 */
void SChatWindow::newConnection()
{
  if (!clientSocket) {
    clientSocket = new ClientSocket(this);
    clientSocket->setProfile(profile);
    connect(clientSocket, SIGNAL(newParticipant(quint16, const QStringList &, bool)), this, SLOT(newParticipant(quint16, const QStringList &, bool)));
    connect(clientSocket, SIGNAL(participantLeft(const QString &)), this, SLOT(participantLeft(const QString &)));
    connect(clientSocket, SIGNAL(newMessage(const QString &, const QString &)), mainChannel, SLOT(msgNewMessage(const QString &, const QString &)));
    connect(clientSocket, SIGNAL(newMessage(const QString &, const QString &)), this, SLOT(newMessage(const QString &, const QString &)));
    connect(clientSocket, SIGNAL(newPrivateMessage(const QString &, const QString &, const QString &)), this, SLOT(newPrivateMessage(const QString &, const QString &, const QString &)));
    connect(clientSocket, SIGNAL(readyForUse()), this, SLOT(readyForUse()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(clientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectionError(QAbstractSocket::SocketError)));
    connect(clientSocket, SIGNAL(changedNick(quint16, const QStringList &)), this, SLOT(changedNick(quint16, const QStringList &)));
    connect(clientSocket, SIGNAL(changedProfile(quint16, const QStringList &, bool)), this, SLOT(changedProfile(quint16, const QStringList &, bool)));
  }
  else {
    state = Ignore;
    clientSocket->quit();
  }
  
  state = WaitingForConnected;
  QString server = settings->network.server();
  
  if (settings->network.isNetwork())
    statusLabel->setText(tr("Идёт подключение к сети %1...").arg(settings->network.name()));
  else
    statusLabel->setText(tr("Идёт подключение к серверу %1...").arg(server));
  
  clientSocket->connectToHost(server, settings->network.port());
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
 * Слот вызывается при изменении профиля в настройках.
 * Инициаторы:
 *   ProfileSettings::save()
 */
void SChatWindow::profileSettingsChanged()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "void SChatWindow::profileSettingsChanged()";
  #endif
  
  if (state == Connected) {
    QStringList list;
    list << profile->nick() << profile->fullName();
    clientSocket->send(sChatOpcodeNewProfile, profile->sex(), list);
  }  
}


/** [private slots]
 * 
 */
void SChatWindow::resetTabNotice(int index)
{
  if (index == -1)
    return;
    
  AbstractTab *tab = static_cast<AbstractTab *>(tabWidget->widget(index));
  if (tab->notice) {
    tabWidget->setTabIcon(index, tab->icon);
    tab->notice = false;
  }
  
  int count = tabWidget->count();
  for (int i = 0; i < count; ++i) {
    AbstractTab *t = static_cast<AbstractTab *>(tabWidget->widget(i));
    if (t->notice)
      return;
  }
  
  if (noticeTimer->isActive()) {
    noticeTimer->stop();
    currentTrayIcon = true;
    trayIcon->setIcon(QIcon(":/images/logo16.png"));
  }
}


/** [private slots]
 * Отправка сообщения в чат
 */
void SChatWindow::returnPressed()
{
  // Получаем текст и удаляем пустые символы по краям
  // Выходим если текс пустой.
  QString text = lineEdit->text().trimmed();
  if (text.isEmpty())
    return;
  
  AbstractTab *tab = static_cast<AbstractTab *>(tabWidget->currentWidget());
  
  // Текст, начинающийся с символа '/' считаем командой.
  // т.к. команды пока не поддерживаются, сообщаем о неизвестной команде.
  if (text.startsWith(QChar('/'))) {
    tab->browser->add(tr("<div style='color:#da251d;'>Неизвестная команда: %1</div>").arg(text.left(text.indexOf(' '))));
  }
  else if (tab->type == AbstractTab::Direct) {
    DirectChannel *ch = static_cast<DirectChannel *>(tab);
    ch->sendText(text);
  }
  else if (tab->type == AbstractTab::DirectServer) {
    DirectChannelServer *ch = static_cast<DirectChannelServer *>(tab);
    ch->sendText(text);
  }
  else if (state == Connected)
    if (tabWidget->currentIndex() == 0)
      clientSocket->send(sChatOpcodeSendMessage, "#main", text);
    else
      clientSocket->send(sChatOpcodeSendMessage, tabWidget->tabText(tabWidget->currentIndex()), text);
  
  lineEdit->clear();  
}


/** [private slots]
 * TODO сделать обвёртку
 */
void SChatWindow::update()
{
  if (!m_updateTimer->isActive())
    m_updateTimer->start();
  
  if (!m_updateNotify) {
    m_updateNotify = new UpdateNotify(this);
    connect(m_updateNotify, SIGNAL(done(int)), SLOT(updateGetDone(int)));
  }
  
  m_updateNotify->execute();
}


/** [private slots]
 * Слот вызывается при завершении работы программы обновления
 */
void SChatWindow::updateGetDone(int code)
{
  if (!m_updateNotify)
    m_updateNotify->deleteLater();
  
  QSettings s(qApp->applicationDirPath() + "/schat.conf", QSettings::IniFormat, this);
  
  if (code == 0) {
    QString version = s.value("Updates/LastDownloadedCoreVersion", "").toString(); 
    trayIcon->showMessage(tr("Доступно обновление до версии %1").arg(version), tr("Щёлкните здесь для того чтобы установить это обновление прямо сейчас."));
  }
  else
    s.setValue("Updates/ReadyToInstall", false);
}


/** [private slots]
 * 
 */
void SChatWindow::settingsPage(int page)
{
  if (isHidden())
    show();
  
  if (!settingsDialog) {
    settingsDialog = new SettingsDialog(profile, settings, this);
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
  
  newConnection();
}


/** [private]
 * 
 */
int SChatWindow::tabIndex(const QString &s, int start) const
{
  int count = tabWidget->count();
  int tab = -1;
  
  if (count > start)
    for (int i = start; i <= count; ++i)
      if (tabWidget->tabText(i) == s) {
        tab = i;
        break;
      }
  
  return tab;
}


/** [private]
 * 
 */
Profile* SChatWindow::profileFromItem(const QStandardItem *item)
{
  return new Profile(quint8(item->data(Qt::UserRole + 1).toUInt()), item->data(Qt::UserRole + 2).toStringList(), this);
}


/** [private]
 * 
 */
QStandardItem* SChatWindow::findItem(const QString &nick) const
{
  QList<QStandardItem *> items;
  
  items = model.findItems(nick, Qt::MatchFixedString);
  if (items.size() == 1)
    return items[0];
  else
    return 0;  
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
  addTabAction = new QAction(QIcon(":/images/tab_new.png"), tr("Новое прямое подключение"), this);
  addTabAction->setShortcut(tr("Ctrl+N"));
  addTabAction->setStatusTip(tr("Открытие новой вкладки, для создания нового прямого подключения"));
  connect(addTabAction, SIGNAL(triggered()), this, SLOT(addTab()));
  
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
  connect(profileSetAction, SIGNAL(triggered()), this, SLOT(settingsProfile()));
  
  // Выход из программы
  quitAction = new QAction(QIcon(":/images/quit.png"), tr("&Выход"), this);
  connect(quitAction, SIGNAL(triggered()), this, SLOT(closeChat()));
  
  // Отправить сообщение в чат
  sendAction = new QAction(QIcon(":/images/send.png"), tr("Отправить, Enter"), this);
  sendAction->setStatusTip(tr("Отправить сообщение в чат"));
  sendButton->setDefaultAction(sendAction);
  connect(sendAction, SIGNAL(triggered()), this, SLOT(returnPressed()));
  
  // Настройка
  settingsAction = new QAction(QIcon(":/images/settings.png"), tr("Настройка..."), this);
  settingsAction->setShortcut(tr("Ctrl+F12"));
  connect(settingsAction, SIGNAL(triggered()), this, SLOT(settingsProfile()));
}


/** [private]
 * 
 */
void SChatWindow::createCornerWidgets()
{
  QToolButton *addTabButton = new QToolButton(this);
  addTabButton->setDefaultAction(addTabAction);
  addTabButton->setAutoRaise(true);
  tabWidget->setCornerWidget(addTabButton, Qt::TopLeftCorner);
  
  QToolButton *closeTabButton = new QToolButton(this);
  closeTabButton->setDefaultAction(closeTabAction);
  closeTabButton->setAutoRaise(true);
  tabWidget->setCornerWidget(closeTabButton, Qt::TopRightCorner);  
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
  
  QToolButton *settingsButton = new QToolButton(this);
  settingsButton->setDefaultAction(settingsAction);
  settingsButton->setAutoRaise(true);
  settingsButton->setMenu(iconMenu);
  
  QToolButton *aboutButton = new QToolButton(this);
  aboutButton->setDefaultAction(aboutAction);
  aboutButton->setAutoRaise(true);
  
  QFrame *line = new QFrame(this);
  line->setFrameShape(QFrame::VLine);
  line->setFrameShadow(QFrame::Sunken);
  
  toolsLayout->addWidget(line);
  toolsLayout->addWidget(settingsButton);
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
  trayIconMenu->addAction(settingsAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(quitAction);
  
  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setIcon(QIcon(":/images/logo16.png"));
  trayIcon->setToolTip(tr("Simple Chat %1").arg(SCHAT_VERSION));
  trayIcon->setContextMenu(trayIconMenu);
  trayIcon->show();
  currentTrayIcon = true;
}


/** [private]
 * 
 */
void SChatWindow::removeConnection()
{
  quint16 err = clientSocket->protocolError();
  model.clear();
  
  // Если ник уже используется, то не показываем выбор сервера
  // и переподключаемся с новым ником, с нулевым интервалом.
  if (err != sChatErrorNickAlreadyUse)
    mainChannel->displayChoiceServer(true);
  
  if (state == Connected || state == Stopped)
    mainChannel->browser->msgDisconnect();
  
  // Если ник отвергнут сервером сообщаем об этом и отключаем авто переподключение.
  if (err == sChatErrorBadNickName) {
    state = Stopped;
    mainChannel->browser->add(tr("<div class='nb'>(%1) <i class='err'>Выбранный ник: <b>%2</b>, не допустим в чате, выберите другой</i></div>")
        .arg(ChatBrowser::currentTime())
        .arg(profile->nick()));
  }
  // Если выбранный ник уже занят, то генерируем новый уникальный ник.
  else if (err == sChatErrorNickAlreadyUse) {
    uniqueNick();
  }

  statusLabel->setText(tr("Не подключено"));
  
  if (!(state == Stopped || state == Ignore)) {
    state = WaitingForConnected;
    
    if (err == sChatErrorNickAlreadyUse)
      newConnection();
    else
      m_reconnectTimer->start();
  }
}


/** [private]
 * 
 */
void SChatWindow::startNotice(int index)
{
  if (index == -1)
    return;
  
  AbstractTab *tab = static_cast<AbstractTab *>(tabWidget->widget(index));
  tab->notice = true;
  tabWidget->setTabIcon(index, QIcon(":/images/notice.png"));
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
  profile->setNick(profile->nick() + QString().setNum(qrand() % 99));
}
