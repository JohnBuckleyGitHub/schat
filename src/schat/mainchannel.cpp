/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "mainchannel.h"

MainChannel::MainChannel(QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  serverLabel = new QLabel(tr("Адрес сервера:"), this);
  serverEdit  = new QLineEdit(this);
  
  serverLabel->setVisible(false);
  serverEdit->setVisible(false);  
  chatText.setFocusPolicy(Qt::NoFocus);
  chatText.setOpenExternalLinks(true);
  
  topLayout = new QHBoxLayout;
  topLayout->addWidget(serverLabel);
  topLayout->addWidget(serverEdit);
  topLayout->addStretch();
  topLayout->setMargin(0);
  
  mainLayout = new QVBoxLayout;
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(&chatText);
  mainLayout->setMargin(0);
  mainLayout->setSpacing(2);
  setLayout(mainLayout);
}


/** [public]
 * 
 */
void MainChannel::append(const QString &message)
{
  chatText.append(message);
  scroll();
}


/** [public]
 * 
 */
void MainChannel::displayChoiceServer(bool display)
{
  if (display) {
    serverLabel->setVisible(true);
    serverEdit->setVisible(true);
    topLayout->setContentsMargins(2, 2, 2, 0);
  }
  else {
    serverLabel->setVisible(false);
    serverEdit->setVisible(false); 
    topLayout->setMargin(0);
  }
}


/** [private]
 * 
 */
void MainChannel::scroll()
{
  QScrollBar *bar = chatText.verticalScrollBar(); 
  bar->setValue(bar->maximum());
}
