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
  
  chatText.setFocusPolicy(Qt::NoFocus);
  chatText.setOpenExternalLinks(true);
  
  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(&chatText);
  mainLayout->setMargin(0);
  setLayout(mainLayout);
}


/** [public]
 * 
 */
void Tab::append(const QString &message)
{
  chatText.append(message);
  scroll();
}

/** [private]
 * 
 */
void Tab::scroll()
{
  QScrollBar *bar = chatText.verticalScrollBar(); 
  bar->setValue(bar->maximum());
}
