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

#include "directchannel.h"
#include "protocol.h"
#include "schatwindow.h"

DirectChannel::DirectChannel(Profile *p, QWidget *parent)
  : AbstractTab(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  type = Direct;
  
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
  mainLayout->addWidget(browser);
  mainLayout->setMargin(0);
  mainLayout->setSpacing(2);
  setLayout(mainLayout);
  
  connect(remoteEdit, SIGNAL(returnPressed()), this, SLOT(newConnection()));
  connect(connectCreateAction, SIGNAL(triggered()), this, SLOT(newConnection()));
  connect(this, SIGNAL(newDirectMessage()), parent, SLOT(newDirectMessage()));
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
  browser->scroll();
}


/** [public]
 * 
 */
void DirectChannel::sendText(const QString &text)
{
  if (state == Connected)
    clientSocket->send(sChatOpcodeSendMessage, remoteNick, text);  
}


/** [public slots]
 * 
 */
void DirectChannel::newParticipant(quint16 /*sex*/, const QStringList &info, bool /*echo*/)
{
  remoteNick = info.at(0);
  browser->setChannel(QChar('#') + remoteNick);
  browser->msg(tr("<i class='green'>Установлено прямое соединение с <b>%2</b>, адрес <b>%3</b></i>")
          .arg(remoteNick)
          .arg(clientSocket->peerAddress().toString()));
}


/** [public slots]
 * 
 */
void DirectChannel::newPrivateMessage(const QString  &/*nick*/, const QString &message, const QString &sender)
{
  browser->msgNewMessage(sender, message);
  
  if (profile->nick() != sender)
    emit newDirectMessage();
}


/** [public slots]
 * 
 */
void DirectChannel::readyForUse()
{
  displayChoiceServer(false);
  state = Connected;
}


/** [private slots]
 * 
 */
void DirectChannel::newConnection()
{
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
  if (state == Connected || state == Stopped) {
    browser->msgDisconnect();
    displayChoiceServer(true);
  }
  else if (state == WaitingForConnected)
    browser->msg(tr("<i class='err'>Не удалось подключится</i></div>"));
  
  clientSocket->deleteLater();  
  state = Disconnected;
}


/** [private]
 * 
 */
void DirectChannel::createActions()
{
  connectCreateButton = new QToolButton(this);
  connectCreateAction = new QAction(QIcon(":/images/connect_creating.png"), tr("Создать новое прямое подключение"), this);
  connectCreateButton->setDefaultAction(connectCreateAction);
  connectCreateButton->setAutoRaise(true);
}
