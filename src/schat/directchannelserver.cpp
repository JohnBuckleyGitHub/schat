/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>
#include <QtNetwork>

#include "directchannelserver.h"

DirectChannelServer::DirectChannelServer(Profile *p, ServerSocket *s, QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  chatBrowser = new ChatBrowser(this);
  socket = s;
  profile = p;
  
  mainLayout = new QVBoxLayout;
  mainLayout->addWidget(chatBrowser);
  mainLayout->setMargin(0);
  setLayout(mainLayout);
  
  initSocket();
}


/** [public]
 * Если сокет активен, то отключимся от хоста.
 * Сокет будет удалён в объекте `Server`.
 */
DirectChannelServer::~DirectChannelServer()
{
  if (socket)
    socket->disconnectFromHost();
}


/** [public]
 * 
 */
void DirectChannelServer::changeSocket(ServerSocket *s)
{
  if (!socket) {
    socket = s;
    initSocket();
  }
}


/** [public]
 * 
 */
void DirectChannelServer::sendText(const QString &text)
{
  #ifdef SCHAT_DEBUG
  qDebug() << "DirectChannelServer::sendText(const QString &text)" << text;
  #endif
  
  if (socket) {
    chatBrowser->msgNewMessage(profile->nick(), text);
    socket->send(sChatOpcodeSendPrivateMessage, profile->nick(), text);
  }
}


/** [private slots]
 * 
 */
void DirectChannelServer::removeConnection()
{
  if (state == Connected) {
    chatBrowser->msgDisconnect();
    if (socket)
      socket->deleteLater();
  }
  
  state = Disconnected;
}


/** [private]
 * 
 */
void DirectChannelServer::initSocket()
{
  if (socket) {
    state = Connected;
    chatBrowser->add(tr("<div class='np'>(%1) <i class='green'>Установлено прямое соединение с <b>%2</b>, адрес <b>%3</b></i></div>")
        .arg(ChatBrowser::currentTime())
        .arg(socket->nick())
        .arg(socket->peerAddress().toString()));
    connect(socket, SIGNAL(newMessage(const QString &, const QString &)), chatBrowser, SLOT(msgNewMessage(const QString &, const QString &)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(removeConnection()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(removeConnection()));
  }
  else
    state = Disconnected;
}
