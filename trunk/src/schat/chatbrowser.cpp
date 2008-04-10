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
  
//  document()->setDefaultStyleSheet("dd");
//  document()->setMaximumBlockCount(10);
}


/** [public]
 * 
 */
void ChatBrowser::add(const QString &message)
{
  append(message);
  scroll();
}


/** [public]
 * 
 */
void ChatBrowser::newMessage(const QString &nick, const QString &message)
{
  append(tr("<div><span style='color:#909090'>[%1] &lt;<b>%2</b>&gt;</span> %3</div>")
      .arg(currentTime())
      .arg(Qt::escape(nick))
      .arg(Qt::escape(message)));
  
  scroll();
}


/** [public]
 * 
 */
void ChatBrowser::newParticipant(quint8 sex, const QString &nick)
{
  if (sex)
    append(tr("<div style='color:#909090'>[%1] <i><b>%2</b> зашла в чат</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  else
    append(tr("<div style='color:#909090'>[%1] <i><b>%2</b> зашёл в чат</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  
  scroll();
}


/** [public]
 * 
 */
void ChatBrowser::participantLeft(quint8 sex, const QString &nick)
{
  if (sex)
    append(tr("<div style='color:#909090'>[%1] <i><b>%2</b> вышла из чата</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  else
    append(tr("<div style='color:#909090'>[%1] <i><b>%2</b> вышел из чата</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  
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
