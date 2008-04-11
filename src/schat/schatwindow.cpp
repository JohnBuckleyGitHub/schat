/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>
#include <QtNetwork>
#include <stdlib.h>

#include "mainchannel.h"
#include "protocol.h"
#include "schatwindow.h"
#include "tab.h"
#include "version.h"
#include "welcomedialog.h"
#include "directchannel.h"
#include "profile.h"
#include "directchannelserver.h"

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
  statusbar     = new QStatusBar(this);
  sendButton    = new QToolButton(centralWidget);
  statusLabel   = new QLabel;
 
  state = Disconnected;
  
  splitter->addWidget(tabWidget);
  splitter->addWidget(rightWidget);
  splitter->setStretchFactor(0, 4);
  splitter->setStretchFactor(1, 1);
  
  rightLayout->addWidget(listView);
  rightLayout->setMargin(0);
  
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
  
  setWindowTitle(tr("Simple Chat"));
  
  tabWidget->setElideMode(Qt::ElideRight);
  listView->setFocusPolicy(Qt::NoFocus);
  listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  listView->setModel(&model);
  
  readSettings();
  createActions();
  createTrayIcon();
  
  connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
  connect(listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addTab(const QModelIndex &)));
  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
  
  mainChannel = new MainChannel(server, this);
  tabWidget->setCurrentIndex(tabWidget->addTab(mainChannel, tr("Общий")));
  tabWidget->setTabIcon(0, QIcon(":/images/main.png"));
  
  if (!hideWelcome || firstRun) {
    welcomeDialog = new WelcomeDialog(profile, this);
    welcomeDialog->setServer(server);
    if (!firstRun)
      welcomeDialog->setHideWelcome(hideWelcome);
    connect(welcomeDialog, SIGNAL(accepted()), this, SLOT(welcomeOk()));
    if (!welcomeDialog->exec())
      mainChannel->displayChoiceServer(true);
  }
  else
    newConnection();
  
  srand(time(NULL));
  
  // Пытаемся запустить сервер, в случае неудачи удаляем сервер.
  daemon = new Server(this);
  if (daemon->start()) {
    daemon->setLocalProfile(profile);
    connect(daemon, SIGNAL(incomingDirectConnection(const QString &, ServerSocket *)), this, SLOT(incomingDirectConnection(const QString &, ServerSocket *)));
  }
  else
    delete daemon;
}


/** [protected]
 * 
 */
void SChatWindow::closeEvent(QCloseEvent *event)
{ // TODO Разобраться с корректным завершением программы.
  writeSettings(); 
  
  if (isHidden()) {
    event->accept();
  }
  else {
    hide();
    event->ignore();
  }
}


/** [public slots]
 * 
 */
void SChatWindow::incomingDirectConnection(const QString &n, ServerSocket *socket)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "SChatWindow::incomingDirectConnection(const QString &n, ServerSocket *socket)" << n;
  #endif
  
  int tab = tabIndex(QChar('#') + n);
  
  if (tab == -1) {
    Profile *p = new Profile(this);
    p->setSex(socket->sex());
    p->fromList(socket->participantInfo());
    tab = tabWidget->addTab(new DirectChannelServer(profile, socket, this), QIcon(Profile::sexIconString(socket->sex())), QChar('#') + n);
    tabWidget->setTabToolTip(tab, p->toolTip());
    p->deleteLater();    
  }
  
  tabWidget->setCurrentIndex(tab);
}


/** [public slots]
 * 
 */
void SChatWindow::newDirectParticipant(quint16 sex, const QStringList &info)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "SChatWindow::newDirectParticipant(quint16 sex, const QStringList &info)" << sex << info;
  #endif
  
  if (DirectChannel *channel  = qobject_cast<DirectChannel *>(sender())) {
    Profile *p = new Profile(this);
    p->setSex(sex);
    p->fromList(info);
    int index = tabWidget->indexOf(channel);
    tabWidget->setTabText(index, QChar('#') + info.at(0));
    tabWidget->setTabToolTip(index, p->toolTip());
    tabWidget->setTabIcon(index, QIcon(Profile::sexIconString(sex)));
    p->deleteLater();
  }  
}


/** [public slots]
 * 
 */
void SChatWindow::newParticipant(quint16 sex, const QStringList &info, bool echo)
{
  QStandardItem *item = new QStandardItem(QIcon(Profile::sexIconString(sex)), info.at(0));
  Profile *p = new Profile(this);
  p->setSex(sex);
  p->fromList(info);  
  item->setData(sex, Qt::UserRole + 1);
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
    if (index != -1) 
      if (Tab *tab = qobject_cast<Tab *>(tabWidget->widget(index)))
        tab->msgNewParticipant(sex, info.at(0));
    
    mainChannel->msgNewParticipant(sex, info.at(0));
  }
}


/** [public slots]
 * 
 */
void SChatWindow::newPrivateMessage(const QString &nick, const QString &message, const QString &sender)
{
  Tab *tab;
  int index = tabIndex(nick);
  
  if (index == -1 ) {
    tab = new Tab(this);
    tabWidget->setCurrentIndex(tabWidget->addTab(tab, nick));
  }
  else 
    tab = qobject_cast<Tab *>(tabWidget->widget(index));
  
  if (tab)
    tab->msgNewMessage(sender, message);
}


/** [public slots]
 * 
 */
void SChatWindow::participantLeft(const QString &nick)
{
  unsigned int sex = 0;
  
  QList<QStandardItem *> items;
  items = model.findItems(nick, Qt::MatchFixedString);
  if (items.size() == 1) {
    sex = items[0]->data(Qt::UserRole + 1).toUInt();
    QModelIndex index = model.indexFromItem(items[0]);
    model.removeRow(index.row());
  }
  
  int index = tabIndex(nick);
  if (index != -1) 
    if (Tab *tab = qobject_cast<Tab *>(tabWidget->widget(index)))
      tab->msgParticipantLeft(sex, nick);
  
  mainChannel->msgParticipantLeft(sex, nick);
}


/** [public slots]
 * 
 */
void SChatWindow::readyForUse()
{
  state = Connected;
  mainChannel->msgReadyForUse(clientSocket->peerAddress().toString());
  statusLabel->setText(tr("Успешно подключены к %1").arg(clientSocket->peerAddress().toString()));
  mainChannel->displayChoiceServer(false);
}


/** [public slots]
 * Слот вызывается из `mainChannel` при необходимости
 * сменить адрес сервера.
 */
void SChatWindow::serverChanged()
{
  statusLabel->setText(tr("Подключение..."));
  
  if (clientSocket)
    clientSocket->quit();
  
  server = mainChannel->server();
  newConnection();
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
    tabWidget->setCurrentIndex(tabWidget->addTab(new DirectChannel(profile, this), label));  
}


/** [private slots]
 * 
 */
void SChatWindow::addTab(const QModelIndex &index)
{
  QString nick = model.itemFromIndex(index)->text();
  int tab = tabIndex(nick);
  
  if (tab == -1)
    tab = tabWidget->addTab(new Tab(this), nick);
  
  tabWidget->setCurrentIndex(tab);
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
  
  writeSettings();
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
      }
      else
        hide();
      
    default:
      break;
  }
}


/** [private slots]
 * 
 */
void SChatWindow::newConnection()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "SChatWindow::newConnection()";
  #endif
  
  state = WaitingForConnected;
  statusLabel->setText(tr("Подключение..."));
  
  if (!clientSocket) {
    clientSocket = new ClientSocket(this);
    connect(clientSocket, SIGNAL(newParticipant(quint16, const QStringList &, bool)), this, SLOT(newParticipant(quint16, const QStringList &, bool)));
    connect(clientSocket, SIGNAL(participantLeft(const QString &)), this, SLOT(participantLeft(const QString &)));
    connect(clientSocket, SIGNAL(newMessage(const QString &, const QString &)), mainChannel, SLOT(msgNewMessage(const QString &, const QString &)));
    connect(clientSocket, SIGNAL(newPrivateMessage(const QString &, const QString &, const QString &)), this, SLOT(newPrivateMessage(const QString &, const QString &, const QString &)));
    connect(clientSocket, SIGNAL(readyForUse()), this, SLOT(readyForUse()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(clientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectionError(QAbstractSocket::SocketError)));
  }
  
  clientSocket->setProfile(profile);
  clientSocket->connectToHost(server, serverPort);
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
  
  QWidget *widget = tabWidget->currentWidget();
  
  // Текст, начинающийся с символа '/' считаем командой.
  // т.к. команды пока не поддерживаются, сообщаем о неизвестной команде.
  if (text.startsWith(QChar('/'))) {
    QString unknownCmd = tr("<div style='color:#da251d;'>Неизвестная команда: %1</div>").arg(text.left(text.indexOf(' ')));
    
    if (MainChannel *tab = qobject_cast<MainChannel *>(widget))
      tab->append(unknownCmd);
    else if (Tab *tab = qobject_cast<Tab *>(widget))
      tab->append(unknownCmd);
    else if (DirectChannel *tab = qobject_cast<DirectChannel *>(widget))
      tab->append(unknownCmd);
  }
  else {    
    if (DirectChannel *tab = qobject_cast<DirectChannel *>(widget))
      tab->sendText(text);
    else if (DirectChannelServer *tab = qobject_cast<DirectChannelServer *>(widget))
      tab->sendText(text);    
    else if (state == Connected)    
      if (tabWidget->currentIndex() == 0)
        clientSocket->send(sChatOpcodeSendMessage, "#main", text);
      else
        clientSocket->send(sChatOpcodeSendMessage, tabWidget->tabText(tabWidget->currentIndex()), text);
  }
  
  lineEdit->clear();  
}


/** [private slots]
 * 
 */
void SChatWindow::welcomeOk()
{
  hideWelcome = welcomeDialog->hideWelcome();
  server = welcomeDialog->server();
  mainChannel->setServer(server);
  welcomeDialog->deleteLater();
  
  newConnection();
}


/** [private]
 * 
 */
int SChatWindow::tabIndex(const QString &s, int start)
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
void SChatWindow::createActions()
{ 
  // Открытие новой вкладки, для создания нового подключения
  QToolButton *addTabButton = new QToolButton(this);
  addTabAction = new QAction(QIcon(":/images/tab_new.png"), tr("Новое прямое подключение, Ctrl+N"), this);
  addTabAction->setShortcut(tr("Ctrl+N"));
  addTabAction->setStatusTip(tr("Открытие новой вкладки, для создания нового прямого подключения"));
  addTabButton->setDefaultAction(addTabAction);
  addTabButton->setAutoRaise(true);
  tabWidget->setCornerWidget(addTabButton, Qt::TopLeftCorner);
  connect(addTabAction, SIGNAL(triggered()), this, SLOT(addTab()));
  
  // Разорвать текущее соединение
  QToolButton *closeTabButton = new QToolButton(this);
  closeTabAction = new QAction(QIcon(":/images/tab_close.png"), tr("Разорвать текущее соединение"), this);
  closeTabAction->setStatusTip(tr("Разорвать текущее соединение"));
  closeTabButton->setDefaultAction(closeTabAction);
  closeTabButton->setAutoRaise(true);
  tabWidget->setCornerWidget(closeTabButton, Qt::TopRightCorner);
  connect(closeTabAction, SIGNAL(triggered()), this, SLOT(closeTab()));
  
  // Отправить сообщение в чат
  sendAction = new QAction(QIcon(":/images/send.png"), tr("Отправить, Enter"), this);
  sendAction->setStatusTip(tr("Отправить сообщение в чат"));
  sendButton->setDefaultAction(sendAction);
  connect(sendAction, SIGNAL(triggered()), this, SLOT(returnPressed()));
  
  // Выход из программы
  quitAction = new QAction(QIcon(":/images/quit.png"), tr("&Выход"), this);
  connect(quitAction, SIGNAL(triggered()), this, SLOT(closeChat()));
}


/** [private]
 * 
 */
void SChatWindow::createTrayIcon()
{
  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(quitAction);
  
  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setIcon(QIcon(":/images/logo16.png"));
  trayIcon->setToolTip(tr("Simple Chat %1").arg(SCHAT_VERSION));
  trayIcon->setContextMenu(trayIconMenu);
  trayIcon->show();
}


/** [private]
 * 
 */
void SChatWindow::readSettings()
{
  QSettings settings(qApp->applicationDirPath() + "/schat.conf", QSettings::IniFormat, this);
  
  QPoint pos = settings.value("Pos", QPoint(-999, -999)).toPoint();
  QSize size = settings.value("Size", QSize(680, 460)).toSize();
  splitter->restoreState(settings.value("Splitter").toByteArray());
  hideWelcome = settings.value("HideWelcome", false).toBool();
  firstRun = settings.value("FirstRun", true).toBool();
  server = settings.value("Server", "192.168.5.130").toString();
  serverPort = quint16(settings.value("ServerPort", 7666).toUInt()); 
  
  resize(size);
  if (pos.x() != -999 && pos.y() != -999)
    move(pos);
  
  settings.beginGroup("Profile");
  profile = new Profile(this);
  profile->setNick(settings.value("Nick", QDir::home().dirName()).toString());
  profile->setFullName(settings.value("Name", "").toString());
  profile->setSex(quint8(settings.value("Sex", 0).toUInt()));
}


/** [private]
 * 
 */
void SChatWindow::removeConnection()
{
  quint16 err = clientSocket->protocolError();
  
  mainChannel->displayChoiceServer(true);
  model.clear();
  
  if (state == Connected || state == Stopped)
    mainChannel->msgDisconnect();
  
  // Если ник отвергнут сервером сообщаем об этом и отключаем авто переподключение.
  if (err == sChatErrorBadNickName) {
    state = Stopped;
    mainChannel->append(tr("<div class='nb'>(%1) <i class='err'>Выбранный ник: <b>%2</b>, не допустим в чате, выберите другой</i></div>")
        .arg(currentTime())
        .arg(profile->nick()));
  }
  // Если выбранный ник уже занят, то генерируем новый уникальный ник.
  else if (err == sChatErrorNickAlreadyUse) {
    uniqueNick();
  }
  
  clientSocket->deleteLater();

  statusLabel->setText(tr("Не подключено"));
  
  if (state != Stopped) {
    state = WaitingForConnected;
    
    if (err == sChatErrorNickAlreadyUse)
      QTimer::singleShot(1000, this, SLOT(newConnection()));
    else
      QTimer::singleShot(reconnectTimeout, this, SLOT(newConnection()));
  }
}


/** [private]
 * 
 */
void SChatWindow::uniqueNick()
{
  profile->setNick(profile->nick() + QString().setNum(rand() % 99));
}


/** [private]
 * 
 */
void SChatWindow::writeSettings()
{
  QSettings settings(qApp->applicationDirPath() + "/schat.conf", QSettings::IniFormat, this);
  
  settings.setValue("Size", size());
  settings.setValue("Pos", pos());
  settings.setValue("Splitter", splitter->saveState());
  settings.setValue("HideWelcome", hideWelcome);
  settings.setValue("FirstRun", false);
  settings.setValue("Server", server);
  settings.setValue("ServerPort", serverPort);
  
  settings.beginGroup("Profile");
  settings.setValue("Nick", profile->nick());
  settings.setValue("Name", profile->fullName());
  settings.setValue("Sex", profile->sex());  
}
