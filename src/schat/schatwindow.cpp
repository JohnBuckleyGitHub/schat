/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>
#include <QtNetwork>

#include "protocol.h"
#include "schatwindow.h"
#include "tab.h"
#include "welcomedialog.h"
#include "version.h"

static const int reconnectTimeout = 3 * 1000;

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
  
  mainChannel = new Tab(this);
  tabWidget->setCurrentIndex(tabWidget->addTab(mainChannel, tr("Общий")));
  tabWidget->setTabIcon(0, QIcon(":/images/main.png"));
  
  if (!hideWelcome || firstRun) {
    welcomeDialog = new WelcomeDialog(nick, fullName, sex, this);
    if (!firstRun)
      welcomeDialog->setHideWelcome(hideWelcome);
    connect(welcomeDialog, SIGNAL(accepted()), this, SLOT(welcomeOk()));
    welcomeDialog->exec();
  }
  else
    newConnection();
}


/** [private]
 * 
 */
void SChatWindow::createActions()
{ 
  // Открытие новой вкладки, для создания нового подключения
  QToolButton *addTabButton = new QToolButton;
  addTabAction = new QAction(QIcon(":/images/tab_new.png"), tr("Новое подключение, Ctrl+N"), this);
  addTabAction->setShortcut(tr("Ctrl+N"));
  addTabAction->setStatusTip(tr("Открытие новой вкладки, для создания нового подключения"));
  addTabButton->setDefaultAction(addTabAction);
  addTabButton->setAutoRaise(true);
  tabWidget->setCornerWidget(addTabButton, Qt::TopLeftCorner);
  connect(addTabAction, SIGNAL(triggered()), this, SLOT(addTab()));
  
  QToolButton *closeTabButton = new QToolButton;
  closeTabAction = new QAction(QIcon(":/images/tab_close.png"), tr("Разорвать текущее соединение"), this);
  closeTabButton->setDefaultAction(closeTabAction);
  closeTabButton->setAutoRaise(true);
  tabWidget->setCornerWidget(closeTabButton, Qt::TopRightCorner);
  connect(closeTabAction, SIGNAL(triggered()), this, SLOT(closeTab()));
  
  sendAction = new QAction(QIcon(":/images/send.png"), tr("Отправить, Enter"), this);
  sendButton->setDefaultAction(sendAction);
  connect(sendAction, SIGNAL(triggered()), this, SLOT(returnPressed()));
  
  quitAction = new QAction(QIcon(":/images/quit.png"), tr("&Выход"), this);
  connect(quitAction, SIGNAL(triggered()), this, SLOT(closeChat()));
}


/** [private slots]
 * 
 */
void SChatWindow::addTab()
{
  qDebug() << "SChatWindow::addTab()";
  tabWidget->setCurrentIndex(tabWidget->addTab(new Tab(this), tr("Новое подключение")));  
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


/** [private slots]
 * 
 */
void SChatWindow::closeTab()
{
  int index = tabWidget->currentIndex();
  if (index)
    tabWidget->removeTab(index); 
}


/** [private slots]
 * 
 */
void SChatWindow::returnPressed()
{
  QString text = lineEdit->text();
  if (text.isEmpty())
    return;

  if (text.startsWith(QChar('/'))) {
    if (Tab *tab = qobject_cast<Tab *>(tabWidget->currentWidget()))
      tab->append(tr("<div style='color:#da251d;'>Неизвестная команда: %1</div>").arg(text.left(text.indexOf(' '))));
  }
  else
    if (tabWidget->currentIndex() == 0)
      clientSocket->send(sChatOpcodeSendMessage, "#main", text);
    else
      clientSocket->send(sChatOpcodeSendMessage, tabWidget->tabText(tabWidget->currentIndex()), text);
  
  lineEdit->clear();  
}


/** [private]
 * 
 */
QString SChatWindow::currentTime()
{
  return QTime::currentTime().toString("hh:mm:ss");
}


/** [private slots]
 * 
 */
void SChatWindow::welcomeOk()
{
  nick = welcomeDialog->nick();
  fullName = welcomeDialog->fullName();
  sex = welcomeDialog->sex();
  hideWelcome = welcomeDialog->hideWelcome();
  welcomeDialog->deleteLater();
  
  newConnection();
}


/** [private slots]
 * 
 */
void SChatWindow::newConnection()
{
  state = WaitingForConnected;
  statusLabel->setText(tr("Подключение..."));
  
  if (!clientSocket)
    clientSocket = new ClientSocket(this);
  
  clientSocket->setNick(nick);
  clientSocket->setFullName(fullName);
  clientSocket->setSex(sex);
  clientSocket->connectToHost("192.168.5.134", 7666);
}


/** [public slots]
 * 
 */
void SChatWindow::readyForUse()
{
  state = Connected;
  statusLabel->setText(tr("Успешно подключены к %1").arg(clientSocket->peerAddress().toString()));
}


/** [public slots]
 * 
 */
void SChatWindow::newParticipant(quint16 sex, const QStringList &info, bool echo)
{
  QString icon;
  if (sex)
    icon = ":/images/female.png";
  else
    icon = ":/images/male.png";
  
  QString name = info.at(1);
  if (name.isEmpty())
    name = tr("&lt;не указано&gt;");
  
  QString userAgent = info.at(2);
  userAgent.replace('/', ' ');
  
  QStandardItem *item = new QStandardItem(QIcon(icon), info.at(0));
  item->setData(sex, Qt::UserRole + 1);
  item->setToolTip(tr("<h3><img src='%1' align='left'> %2</h3>"
                      "<table><tr><td>Настоящее имя:</td><td>%3</td></tr>"
                      "<tr><td>Клиент:</td><td>%4</td></tr>"
                      "<tr><td>IP-адрес:</td><td>%5</td></tr></table>")
                      .arg(icon).arg(info.at(0)).arg(name).arg(userAgent).arg(info.at(3)));  
  
  model.appendRow(item);
  model.sort(0);

  if (echo) {
    QString line;
    if (sex)
      line = tr("<div style='color:#909090'>[%1] <i><b>%2</b> зашла в чат</i></div>").arg(currentTime()).arg(Qt::escape(info.at(0)));
    else
      line = tr("<div style='color:#909090'>[%1] <i><b>%2</b> зашёл в чат</i></div>").arg(currentTime()).arg(Qt::escape(info.at(0)));
    int index = tabIndex(info.at(0));
    if (index != -1) 
      if (Tab *tab = qobject_cast<Tab *>(tabWidget->widget(index)))
        tab->append(line);
    
    mainChannel->append(line);
  }
}


/** [public slots]
 * 
 */
void SChatWindow::newMessage(const QString &nick, const QString &message)
{
  mainChannel->append(tr("<div><span style='color:#909090'>[%1] &lt;<b>%2</b>&gt;</span> %3</div>")
      .arg(currentTime())
      .arg(Qt::escape(nick))
      .arg(message));
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
    tab->append(tr("<div><span style='color:#909090'>[%1] &lt;<b>%2</b>&gt;</span> %3</div>")
        .arg(currentTime())
        .arg(Qt::escape(sender))
        .arg(message));
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
  
  QString line;
  if (sex)
    line = tr("<div style='color:#909090'>[%1] <i><b>%2</b> вышла из чата</i></div>").arg(currentTime()).arg(Qt::escape(nick));
  else
    line = tr("<div style='color:#909090'>[%1] <i><b>%2</b> вышел из чата</i></div>").arg(currentTime()).arg(Qt::escape(nick));
  int index = tabIndex(nick);
  if (index != -1) 
    if (Tab *tab = qobject_cast<Tab *>(tabWidget->widget(index)))
      tab->append(line);
  
  mainChannel->append(line);
}


/** [private slots]
 * 
 */
void SChatWindow::disconnected()
{
  qDebug() << "SChatWindow::disconnected()";
  
  if (ClientSocket *socket = qobject_cast<ClientSocket *>(sender()))
    removeConnection(socket);
}


/** [private slots]
 * 
 */
void SChatWindow::connectionError(QAbstractSocket::SocketError /* socketError */)
{
  qDebug() << "SChatWindow::connectionError(QAbstractSocket::SocketError /* socketError */)";
  
  if (ClientSocket *socket = qobject_cast<ClientSocket *>(sender())) {
    qDebug() << "ERROR:" << socket->errorString();
    removeConnection(socket);
  }
}


/** [private]
 * 
 */
void SChatWindow::removeConnection(ClientSocket *socket)
{
  if (state == Connected)
    mainChannel->append(tr("<div style='color:#da251d;'>[%1] <i>Соединение разорвано</i></div>").arg(currentTime()));
  
  model.clear();
  socket->deleteLater();
  
  state = WaitingForConnected;
  statusLabel->setText(tr("Не подключено"));
  
  QTimer::singleShot(reconnectTimeout, this, SLOT(newConnection()));
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


/** [private slots]
 * 
 */
void SChatWindow::closeChat()
{
  writeSettings();
  qApp->quit();
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


/** [private]
 * 
 */
void SChatWindow::readSettings()
{
  QSettings settings(qApp->applicationDirPath() + "/schat.ini", QSettings::IniFormat, this);
  
  QPoint pos = settings.value("Pos", QPoint(-999, -999)).toPoint();
  QSize size = settings.value("Size", QSize(680, 460)).toSize();
  splitter->restoreState(settings.value("Splitter").toByteArray());
  hideWelcome = settings.value("HideWelcome", false).toBool();
  firstRun = settings.value("FirstRun", true).toBool();
  
  resize(size);
  if (pos.x() != -999 && pos.y() != -999)
    move(pos);
  
  settings.beginGroup("Profile");
  nick = settings.value("Nick", QDir::home().dirName()).toString();
  fullName = settings.value("Name", "").toString();
  sex = quint8(settings.value("Sex", 0).toUInt());

}


/** [private]
 * 
 */
void SChatWindow::writeSettings()
{
  QSettings settings(qApp->applicationDirPath() + "/schat.ini", QSettings::IniFormat, this);
  
  settings.setValue("size", size());
  settings.setValue("pos", pos());
  settings.setValue("splitter", splitter->saveState());
  settings.setValue("hideWelcome", hideWelcome);
  settings.setValue("FirstRun", false);
  
  settings.beginGroup("Profile");
  settings.setValue("nick", nick);
  settings.setValue("name", fullName);
  settings.setValue("sex", sex);  
}
