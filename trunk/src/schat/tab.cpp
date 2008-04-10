/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "tab.h"

Tab::Tab(QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  chatBrowser = new ChatBrowser(this);
  
  mainLayout = new QVBoxLayout;
  mainLayout->addWidget(chatBrowser);
  mainLayout->setMargin(0);
  setLayout(mainLayout);
}

Tab::~Tab()
{
  delete chatBrowser;
}
