/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "tab.h"

Tab::Tab(QWidget *parent)
  : AbstractTab(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  type = Private;
  
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(browser);
  mainLayout->setMargin(0);
}
