/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "mainchannel.h"
#include "networkwidget.h"
#include "settings.h"

MainChannel::MainChannel(Settings *settings, QWidget *parent)
  : AbstractTab(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  type = Main;
  
  m_settings = settings;
  
  m_serverLabel = new QLabel(tr("Адрес сервера:"), this);
  m_serverEdit  = new QLineEdit(m_settings->network.server(), this);
  
  createActions();
  
  m_serverLabel->setVisible(false);
  m_serverEdit->setVisible(false);
  m_connectCreateButton->setVisible(false);
  
  m_networkWidget = new NetworkWidget(settings, this);
  m_networkWidget->setVisible(false);
  
  m_topLayout = new QHBoxLayout;
  m_topLayout->addWidget(m_serverLabel);
  m_topLayout->addWidget(m_serverEdit);
//  m_topLayout->addWidget(m_connectCreateButton);
  m_topLayout->addStretch();
  m_topLayout->setMargin(0);
  
  m_networkLayout = new QHBoxLayout;
  m_networkLayout->addWidget(m_networkWidget);
  m_networkLayout->addWidget(m_connectCreateButton);
  m_networkLayout->addStretch();
  m_networkLayout->setMargin(0);
  
  m_mainLayout = new QVBoxLayout;
  m_mainLayout->addLayout(m_networkLayout);
  m_mainLayout->addLayout(m_topLayout);
  m_mainLayout->addWidget(browser);
  m_mainLayout->setMargin(0);
  m_mainLayout->setSpacing(2);
  setLayout(m_mainLayout);
  
  connect(m_serverEdit, SIGNAL(returnPressed()), parent, SLOT(serverChanged()));
  connect(m_connectCreateAction, SIGNAL(triggered()), parent, SLOT(serverChanged()));
}


/** [public]
 * 
 */
void MainChannel::displayChoiceServer(bool display)
{
  if (display) {
    m_topLayout->setContentsMargins(4, 2, 4, 0);
    m_networkLayout->setContentsMargins(4, 2, 4, 0);
    m_serverLabel->setVisible(true);
    m_serverEdit->setVisible(true);
    m_networkWidget->setVisible(true);
    m_connectCreateButton->setVisible(true);
  }
  else {
    m_serverLabel->setVisible(false);
    m_serverEdit->setVisible(false);
    m_connectCreateButton->setVisible(false);
    m_networkWidget->setVisible(false);
    m_topLayout->setMargin(0);
  }
  browser->scroll();
}


/** [private]
 * 
 */
void MainChannel::createActions()
{
  m_connectCreateButton = new QToolButton(this);
  m_connectCreateAction = new QAction(QIcon(":/images/connect_creating.png"), tr("Подключится к серверу"), this);
  m_connectCreateButton->setDefaultAction(m_connectCreateAction);
  m_connectCreateButton->setAutoRaise(true);
}
