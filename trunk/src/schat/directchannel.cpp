/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "directchannel.h"

DirectChannel::DirectChannel(QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  serverLabel = new QLabel(tr("Адрес:"), this);
  serverEdit  = new QLineEdit(this);
  
  createActions();
  
//  serverLabel->setVisible(false);
//  serverEdit->setVisible(false);
//  connectCreateButton->setVisible(false);
  chatText.setFocusPolicy(Qt::NoFocus);
  chatText.setOpenExternalLinks(true);
  
  topLayout = new QHBoxLayout;
  topLayout->addWidget(serverLabel);
  topLayout->addWidget(serverEdit);
  topLayout->addWidget(connectCreateButton);
  topLayout->addStretch();
  topLayout->setContentsMargins(4, 2, 4, 0);
  
  mainLayout = new QVBoxLayout;
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(&chatText);
  mainLayout->setMargin(0);
  mainLayout->setSpacing(2);
  setLayout(mainLayout);
  
//  connect(serverEdit, SIGNAL(returnPressed()), parent, SLOT(serverChanged()));
//  connect(connectCreateAction, SIGNAL(triggered()), parent, SLOT(serverChanged()));
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
void DirectChannel::displayChoiceServer(bool display)
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
void DirectChannel::scroll()
{
  QScrollBar *bar = chatText.verticalScrollBar(); 
  bar->setValue(bar->maximum());
}
