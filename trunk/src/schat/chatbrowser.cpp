/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "chatbrowser.h"


/** [public]
 * 
 */
ChatBrowser::ChatBrowser(QWidget *parent)
  : QTextBrowser(parent)
{
  setFocusPolicy(Qt::NoFocus);
  setOpenExternalLinks(true);
}


/** [public]
 * 
 */
void ChatBrowser::add(const QString &message)
{
  append(message);
  scroll();
}


/** [private]
 * 
 */
void ChatBrowser::scroll()
{
  QScrollBar *bar = verticalScrollBar();
  bar->setValue(bar->maximum());
}
