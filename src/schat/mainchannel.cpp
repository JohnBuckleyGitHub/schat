/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "mainchannel.h"
#include "networkwidget.h"
#include "settings.h"

/*!
 * \class MainChannel
 * \brief Обеспечивает поддержку главного канала чата.
 */

/*!
 * \brief Конструктор класса MainChannel.
 */
MainChannel::MainChannel(Settings *settings, QWidget *parent)
  : AbstractTab(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  type = Main;
  
  m_settings = settings;
  browser->setChannel("#main");
  
  createActions();
  
  m_connectCreateButton->setVisible(false);
  
  m_networkWidget = new NetworkWidget(settings, this);
  m_networkWidget->setVisible(false);
  
  m_networkLayout = new QHBoxLayout;
  m_networkLayout->addWidget(m_networkWidget);
  m_networkLayout->addWidget(m_connectCreateButton);
  m_networkLayout->addStretch();
  m_networkLayout->setMargin(0);
  
  m_mainLayout = new QVBoxLayout;
  m_mainLayout->addLayout(m_networkLayout);
  m_mainLayout->addWidget(browser);
  m_mainLayout->setMargin(0);
  m_mainLayout->setSpacing(2);
  setLayout(m_mainLayout);
}


/** [public]
 * 
 */
void MainChannel::displayChoiceServer(bool display)
{
  if (display) {
    m_networkLayout->setContentsMargins(4, 2, 4, 0);
    m_networkWidget->setVisible(true);
    m_connectCreateButton->setVisible(true);
  }
  else {
    m_connectCreateButton->setVisible(false);
    m_networkWidget->setVisible(false);
    m_networkLayout->setMargin(0);
  }
  browser->scroll();
}


/** [private slots]
 * 
 */
void MainChannel::serverChanged()
{
  m_networkWidget->save();
  m_settings->notify(Settings::ServerChanged);
}


/** [private]
 * 
 */
void MainChannel::createActions()
{
  m_connectCreateButton = new QToolButton(this);
  m_connectCreateAction = new QAction(QIcon(":/images/connect_creating.png"), tr("Подключится к сети или одиночному серверу"), this);
  m_connectCreateButton->setDefaultAction(m_connectCreateAction);
  m_connectCreateButton->setAutoRaise(true);
  connect(m_connectCreateAction, SIGNAL(triggered()), this, SLOT(serverChanged()));
}
