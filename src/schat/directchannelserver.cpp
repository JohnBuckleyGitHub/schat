/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "directchannelserver.h"

DirectChannelServer::DirectChannelServer(ServerSocket *s, QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  chatBrowser = new ChatBrowser(this);
  socket = s;
  
  mainLayout = new QVBoxLayout;
  mainLayout->addWidget(chatBrowser);
  mainLayout->setMargin(0);
  setLayout(mainLayout);
  
  if (socket)
    connect(socket, SIGNAL(newMessage(const QString &, const QString &)), chatBrowser, SLOT(newMessage(const QString &, const QString &)));
}
