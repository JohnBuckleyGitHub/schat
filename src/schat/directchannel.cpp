/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>
#include <QtNetwork>

#include "directchannel.h"
#include "protocol.h"
#include "schatwindow.h"

DirectChannel::DirectChannel(Profile *p, QWidget *parent)
  : QWidget(parent)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "DirectChannel::DirectChannel(Profile *p, QWidget *parent)";
  #endif
  
  setAttribute(Qt::WA_DeleteOnClose);
  
  chatBrowser = new ChatBrowser(this);  
  chat        = qobject_cast<SChatWindow *>(parent);
  profile     = p;  
  adrLabel    = new QLabel(tr("Адрес:"), this);
  remoteEdit  = new QLineEdit(this);
  
  state = Disconnected;
  
  createActions();
  
  topLayout = new QHBoxLayout;
  topLayout->addWidget(adrLabel);
  topLayout->addWidget(remoteEdit);
  topLayout->addWidget(connectCreateButton);
  topLayout->addStretch();
  
  mainLayout = new QVBoxLayout;
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(chatBrowser);
  mainLayout->setMargin(0);
  mainLayout->setSpacing(2);
  setLayout(mainLayout);
  
  connect(remoteEdit, SIGNAL(returnPressed()), this, SLOT(newConnection()));
  connect(connectCreateAction, SIGNAL(triggered()), this, SLOT(newConnection()));
  connect(this, SIGNAL(newDirectParticipant(quint16, const QStringList &)), parent, SLOT(newDirectParticipant(quint16, const QStringList &)));
  
  displayChoiceServer(true);
}


/** [public]
 * 
 */
void DirectChannel::displayChoiceServer(bool display)
{
  if (display) {
    topLayout->setContentsMargins(4, 2, 4, 0);
    adrLabel->setVisible(true);
    remoteEdit->setVisible(true);
    connectCreateButton->setVisible(true);
  }
  else {
    adrLabel->setVisible(false);
    remoteEdit->setVisible(false);
    connectCreateButton->setVisible(false);
    topLayout->setMargin(0);
  }
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


/** [public slots]
 * 
 */
void DirectChannel::newParticipant(quint16 /*sex*/, const QStringList &info, bool /*echo*/)
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
  displayChoiceServer(false);
  state = Connected;
  chatBrowser->add(tr("<div class='np'>(%1) <i class='green'>Установлено прямое соединение с <b>%2</b></i></div>")
          .arg(ChatBrowser::currentTime())
          .arg(clientSocket->peerAddress().toString()));
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
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(removeConnection()));
    connect(clientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(removeConnection()));
  }
  
  clientSocket->setDirect(true);
  clientSocket->setProfile(profile);
  clientSocket->connectToHost(remoteEdit->text(), 7666);
}


/** [private slots]
 * 
 */
void DirectChannel::removeConnection()
{
  #ifdef SCHAT_DEBUG
  qDebug() << "DirectChannel::removeConnection(ClientSocket *socket)" << state;
  #endif
  
  if (state == Connected || state == Stopped) {
    chatBrowser->msgDisconnect();
    displayChoiceServer(true);
  }
  else
    chatBrowser->add(tr("<div class='np'>(%1) <i class='err'>Не удалось подключится</i></div>").arg(ChatBrowser::currentTime()));
  
  clientSocket->deleteLater();  
  state = Disconnected;
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
