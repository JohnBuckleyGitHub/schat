/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>
#include <QtNetwork>

#include "protocol.h"
#include "schatwindow.h"
#include "tab.h"

SChatWindow::SChatWindow(QWidget *parent)
  : QMainWindow(parent)
{
  resize(680, 460);
  
  centralWidget = new QWidget(this);  
  splitter      = new QSplitter(centralWidget);
  lineEdit      = new QLineEdit(centralWidget);
  tabWidget     = new QTabWidget(splitter);
  rightWidget   = new QWidget(splitter);
  listView      = new QListView(rightWidget);  
  rightLayout   = new QVBoxLayout(rightWidget);
  mainLayout    = new QVBoxLayout(centralWidget);
  sendLayout    = new QHBoxLayout(centralWidget);
  statusbar     = new QStatusBar(this);
  sendButton    = new QToolButton(centralWidget);
  
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
  
  setCentralWidget(centralWidget);
  setStatusBar(statusbar);
  
  setWindowTitle(tr("Simple Chat"));
  
  tabWidget->setElideMode(Qt::ElideRight);
  listView->setFocusPolicy(Qt::NoFocus);
  listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  listView->setModel(&model);

  connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
  connect(listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(addTab(const QModelIndex &)));
  
  clientSocket = new ClientSocket(this);
  
  createActions();
  
  mainChannel = new Tab(this);
  tabWidget->setCurrentIndex(tabWidget->addTab(mainChannel, "Общий"));
  tabWidget->setTabIcon(0, QIcon(":/images/main.png"));
  tabWidget->setContentsMargins(0,0,0,0);
  
  bool ok;
  nick = QInputDialog::getText(this, tr("Ваше имя"),
                                           tr("Введите ваше имя:"), QLineEdit::Normal,
                                           QDir::home().dirName(), &ok);
  if (ok && !nick.isEmpty())
    newConnection();
}


/** [private]
 * 
 */
void SChatWindow::createActions()
{
  QToolButton *addTabButton = new QToolButton;
  addTabAction = new QAction(QIcon(":/images/tab_new.png"), tr("Новое подключение, Ctrl+N"), this);
  addTabAction->setShortcut(tr("Ctrl+N"));
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
}


/** [private slots]
 * 
 */
void SChatWindow::addTab()
{
  qDebug() << "SChatWindow::addTab()";
  tabWidget->setCurrentIndex(tabWidget->addTab(new Tab(this), "Новое подключение"));  
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
      tab->append(tr("<div style='color:#da251d;'>! Неизвестная команда: %1</div>").arg(text.left(text.indexOf(' '))));
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

void SChatWindow::newConnection()
{
  clientSocket->setNick(nick);
  clientSocket->connectToHost("192.168.5.134", 7666);
}


/** [public slots]
 * 
 */
void SChatWindow::newParticipant(const QString &p, bool echo)
{
  QStandardItem *item = new QStandardItem(QIcon(":/images/male.png"), p);
  model.appendRow(item);
  model.sort(0);

  if (echo) {
    QString line = tr("<div style='color:#909090'>[%1] <i><b>%2</b> заходит в чат</i></div>").arg(currentTime()).arg(Qt::escape(p));
    int index = tabIndex(p);
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
  qDebug() << "SChatWindow::participantLeft(const QString &nick)";
  
  QList<QStandardItem *> items;
  items = model.findItems(nick, Qt::MatchFixedString);
  if (items.size() == 1) {
    QModelIndex index = model.indexFromItem(items[0]);
    model.removeRow(index.row());
  }
  
  QString line = tr("<div style='color:#909090'>[%1] <i><b>%2</b> выходит из чата</i></div>").arg(currentTime()).arg(Qt::escape(nick));
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
  
  if (ClientSocket *socket = qobject_cast<ClientSocket *>(sender())) {
    mainChannel->append(tr("<div style='color:#da251d;'>[%1] <i>Соединение разорвано</i></div>").arg(currentTime()));
    removeConnection(socket);
  }
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
  qDebug() << "SChatWindow::removeConnection(ClientSocket *socket)";
  model.clear();
  
  socket->deleteLater();
}
