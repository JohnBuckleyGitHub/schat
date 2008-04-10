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

DirectChannel::DirectChannel(Profile *p, QWidget *parent)
  : QWidget(parent)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "DirectChannel::DirectChannel(Profile *p, QWidget *parent)";
  #endif
  
  setAttribute(Qt::WA_DeleteOnClose);
  
  chatBrowser = new ChatBrowser(this);
  
  chat = qobject_cast<SChatWindow *>(parent);
  profile = p;
  
  adrLabel    = new QLabel(tr("Адрес:"), this);
  remoteEdit  = new QLineEdit(this);
  
  state = Disconnected;
  
  createActions();
  
  topLayout = new QHBoxLayout;
  topLayout->addWidget(adrLabel);
  topLayout->addWidget(remoteEdit);
  topLayout->addWidget(connectCreateButton);
  topLayout->addStretch();
  topLayout->setContentsMargins(4, 2, 4, 0);
  
  mainLayout = new QVBoxLayout;
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(chatBrowser);
  mainLayout->setMargin(0);
  mainLayout->setSpacing(2);
  setLayout(mainLayout);
  
  connect(remoteEdit, SIGNAL(returnPressed()), this, SLOT(newConnection()));
  connect(connectCreateAction, SIGNAL(triggered()), this, SLOT(newConnection()));
  connect(this, SIGNAL(newDirectParticipant(quint16, const QStringList &)), parent, SLOT(newDirectParticipant(quint16, const QStringList &)));
  
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
    clientSocket->send(sChatOpcodeSendMessage, remoteNick, text);  
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
void DirectChannel::newParticipant(quint16 sex, const QStringList &info, bool echo)
{
  remoteNick = info.at(0);
}


/** [public slots]
 * 
 */
void DirectChannel::newPrivateMessage(const QString  &/*nick*/, const QString &message, const QString &sender)
{
  chatBrowser->msgNewMessage(sender, message);
}


/** [public slots]
 * 
 */
void DirectChannel::readyForUse()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "DirectChannel::readyForUse()" << clientSocket->peerAddress().toString();
  #endif
  
  state = Connected;
  chatBrowser->msgReadyForUse(clientSocket->peerAddress().toString());
}


/** [private slots]
 * 
 */
void DirectChannel::newConnection()
{
//  #ifdef SCHAT_DEBUG
//  qDebug() << "DirectChannel::newConnection()" << remoteEdit->text() << chat->getNick() << chat->getFullName() << chat->getSex();
//  #endif

  state = WaitingForConnected;

  if (!clientSocket) {
    clientSocket = new ClientSocket(this);
    connect(clientSocket, SIGNAL(newParticipant(quint16, const QStringList &, bool)), this, SIGNAL(newDirectParticipant(quint16, const QStringList &)));
    connect(clientSocket, SIGNAL(newParticipant(quint16, const QStringList &, bool)), this, SLOT(newParticipant(quint16, const QStringList &, bool)));
    connect(clientSocket, SIGNAL(newPrivateMessage(const QString &, const QString &, const QString &)), this, SLOT(newPrivateMessage(const QString &, const QString &, const QString &)));
    connect(clientSocket, SIGNAL(readyForUse()), this, SLOT(readyForUse()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(clientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectionError(QAbstractSocket::SocketError)));
  }
  
  clientSocket->setDirect(true);
  clientSocket->setProfile(profile);
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
    chatBrowser->msgDisconnect();
  
  clientSocket->deleteLater();
  
  if (state != Stopped) {
    state = WaitingForConnected;
    QTimer::singleShot(reconnectTimeout, this, SLOT(newConnection()));
  }
}
