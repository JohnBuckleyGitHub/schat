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

#include "directchannelserver.h"

DirectChannelServer::DirectChannelServer(Profile *p, ServerSocket *s, QWidget *parent)
  : AbstractTab(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  type = DirectServer;
  
  socket = s;
  profile = p;
  
  mainLayout = new QVBoxLayout;
  mainLayout->addWidget(browser);
  mainLayout->setMargin(0);
  setLayout(mainLayout);
  
  initSocket();
  
  connect(this, SIGNAL(newDirectMessage()), parent, SLOT(newDirectMessage()));
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
  if (socket) {
    browser->msgNewMessage(profile->nick(), text);
    socket->send(sChatOpcodeSendPrivateMessage, profile->nick(), text);
  }
}


/** [private slots]
 * 
 */
void DirectChannelServer::newMessage(const QString &nick, const QString &message)
{
  browser->msgNewMessage(nick, message);
  emit newDirectMessage();
}


/** [private slots]
 * 
 */
void DirectChannelServer::removeConnection()
{
  if (state == Connected) {
    browser->msgDisconnect();
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
    browser->setChannel(QChar('#') + socket->nick());
    state = Connected;
    browser->msg(tr("<i class='green'>Установлено прямое соединение с <b>%2</b>, адрес <b>%3</b></i>")
        .arg(socket->nick())
        .arg(socket->peerAddress().toString()));
    connect(socket, SIGNAL(newMessage(const QString &, const QString &)), this, SLOT(newMessage(const QString &, const QString &)));
    connect(socket, SIGNAL(disconnected()), this, SLOT(removeConnection()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(removeConnection()));
  }
  else
    state = Disconnected;
}
