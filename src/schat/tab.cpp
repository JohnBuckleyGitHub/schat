/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "tab.h"

Tab::Tab(QWidget *parent)
  : QWidget(parent)
{
  chatText.setFocusPolicy(Qt::NoFocus);
  chatText.setOpenExternalLinks(true);
  
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(&chatText);
  mainLayout->setMargin(0);
  setLayout(mainLayout);
}
