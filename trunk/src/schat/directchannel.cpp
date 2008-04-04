/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>
#include <QtNetwork>

#include "directchannel.h"
#include "protocol.h"
#include "schatwindow.h"

static const int reconnectTimeout = 3 * 1000;

DirectChannel::DirectChannel(QWidget *parent)
  : QWidget(parent)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "DirectChannel::DirectChannel(QWidget *parent)";
  #endif
  
  setAttribute(Qt::WA_DeleteOnClose);
  
  chat = qobject_cast<SChatWindow *>(parent);
  
  adrLabel    = new QLabel(tr("Адрес:"), this);
  remoteEdit  = new QLineEdit(this);
  
  state = Disconnected;
  
  createActions();
  
  chatText.setFocusPolicy(Qt::NoFocus);
  chatText.setOpenExternalLinks(true);
  
  topLayout = new QHBoxLayout;
  topLayout->addWidget(adrLabel);
  topLayout->addWidget(remoteEdit);
  topLayout->addWidget(connectCreateButton);
  topLayout->addStretch();
  topLayout->setContentsMargins(4, 2, 4, 0);
  
  mainLayout = new QVBoxLayout;
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(&chatText);
  mainLayout->setMargin(0);
  mainLayout->setSpacing(2);
  setLayout(mainLayout);
  
  connect(remoteEdit, SIGNAL(returnPressed()), this, SLOT(newConnection()));
  connect(connectCreateAction, SIGNAL(triggered()), this, SLOT(newConnection()));
  
}


/** [public]
 * 
 */
void DirectChannel::append(const QString &message)
{
  chatText.append(message);
  scroll();
}


/** [public]
 * 
 */
void DirectChannel::sendText(const QString &text)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "DirectChannel::sendText(const QString &text)" << text;
  #endif
  
  if (state == Connected)
    clientSocket->send(sChatOpcodeSendMessage, "#main", text);
  
}


/** [private slots]
 * 
 */
void DirectChannel::connectionError(QAbstractSocket::SocketError /* socketError */)
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
void DirectChannel::disconnected()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "DirectChannel::disconnected()";
  #endif
  
  removeConnection();
}


/** [public slots]
 * 
 */
void DirectChannel::newPrivateMessage(const QString &nick, const QString &message, const QString &sender)
{
  append(tr("<div><span style='color:#909090'>[%1] &lt;<b>%2</b>&gt;</span> %3</div>")
        .arg(currentTime())
        .arg(Qt::escape(sender))
        .arg(message));
}


/** [public slots]
 * 
 */
void DirectChannel::readyForUse()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "DirectChannel::readyForUse()" << clientSocket->peerAddress().toString();
  #endif
  
  QString statusText = tr("Успешно подключены к %1").arg(clientSocket->peerAddress().toString());
  
  state = Connected;
  append(tr("<div><span style='color:#909090'>[%1]</span> <i style='color:#6bb521;'>%2</i></div>").arg(currentTime()).arg(statusText));
}


/** [private slots]
 * 
 */
void DirectChannel::newConnection()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "DirectChannel::newConnection()" << remoteEdit->text() << chat->getNick() << chat->getFullName() << chat->getSex();
  #endif

  state = WaitingForConnected;

  if (!clientSocket) {
    clientSocket = new ClientSocket(this);
    connect(clientSocket, SIGNAL(newPrivateMessage(const QString &, const QString &, const QString &)), this, SLOT(newPrivateMessage(const QString &, const QString &, const QString &)));
    connect(clientSocket, SIGNAL(readyForUse()), this, SLOT(readyForUse()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(clientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectionError(QAbstractSocket::SocketError)));
  }
  
  clientSocket->setNick('#' + chat->getNick());
  clientSocket->setFullName(chat->getFullName());
  clientSocket->setSex(chat->getSex());
  clientSocket->connectToHost(remoteEdit->text(), 7666);
}


/** [private]
 * 
 */
void DirectChannel::createActions()
{
  connectCreateButton = new QToolButton(this);
  connectCreateAction = new QAction(QIcon(":/images/connect_creating.png"), tr("Подключится к серверу"), this);
  connectCreateButton->setDefaultAction(connectCreateAction);
  connectCreateButton->setAutoRaise(true);
}


/** [private]
 * 
 */
void DirectChannel::removeConnection()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "DirectChannel::removeConnection(ClientSocket *socket)" << state;
  #endif
  
  if (state == Connected || state == Stopped)
    append(tr("<div><span style='color:#909090'>[%1]</span> <i style='color:#da251d;'>Соединение разорвано</i></div>").arg(currentTime()));
  
  clientSocket->deleteLater();
  
  if (state != Stopped) {
    state = WaitingForConnected;
    QTimer::singleShot(reconnectTimeout, this, SLOT(newConnection()));
  }
}


/** [private]
 * 
 */
void DirectChannel::scroll()
{
  QScrollBar *bar = chatText.verticalScrollBar(); 
  bar->setValue(bar->maximum());
}
