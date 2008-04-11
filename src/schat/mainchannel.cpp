/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "mainchannel.h"

MainChannel::MainChannel(const QString &server, QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  chatBrowser = new ChatBrowser(this);
  
  serverLabel = new QLabel(tr("Адрес сервера:"), this);
  serverEdit  = new QLineEdit(server, this);
  
  createActions();
  
  serverLabel->setVisible(false);
  serverEdit->setVisible(false);
  connectCreateButton->setVisible(false);
  
  topLayout = new QHBoxLayout;
  topLayout->addWidget(serverLabel);
  topLayout->addWidget(serverEdit);
  topLayout->addWidget(connectCreateButton);
  topLayout->addStretch();
  topLayout->setMargin(0);
  
  mainLayout = new QVBoxLayout;
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(chatBrowser);
  mainLayout->setMargin(0);
  mainLayout->setSpacing(2);
  setLayout(mainLayout);
  
  connect(serverEdit, SIGNAL(returnPressed()), parent, SLOT(serverChanged()));
  connect(connectCreateAction, SIGNAL(triggered()), parent, SLOT(serverChanged()));
}


/** [public]
 * 
 */
void MainChannel::displayChoiceServer(bool display)
{
  if (display) {
    topLayout->setContentsMargins(4, 2, 4, 0);
    serverLabel->setVisible(true);
    serverEdit->setVisible(true);
    connectCreateButton->setVisible(true);
  }
  else {
    serverLabel->setVisible(false);
    serverEdit->setVisible(false);
    connectCreateButton->setVisible(false);
    topLayout->setMargin(0);
  }
}


/** [private]
 * 
 */
void MainChannel::createActions()
{
  connectCreateButton = new QToolButton(this);
  connectCreateAction = new QAction(QIcon(":/images/connect_creating.png"), tr("Создать новое прямое подключение"), this);
  connectCreateButton->setDefaultAction(connectCreateAction);
  connectCreateButton->setAutoRaise(true);
}
