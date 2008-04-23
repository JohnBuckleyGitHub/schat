/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "abstracttab.h"

AbstractTab::AbstractTab(QWidget *parent)
  : QWidget(parent)
{
  setAttribute(Qt::WA_DeleteOnClose);
  
  browser = new ChatBrowser(this);
  type = Unknown;
}
