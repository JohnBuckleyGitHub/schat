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
  document()->setDefaultStyleSheet(
      "div.np, div.nb { color:#909090; font-size:small; }"
      "div.nb i { font-size:medium; }"
      ".gr { color:#909090; }"
      ".green { color:#6bb521; }"
      ".err { color:#da251d; }"
      ".info { color:#003399; }");
}


/** [public]
 * Добавляет строку.
 */
void ChatBrowser::add(const QString &message)
{
  append(message);
  scroll();
}


/** [public]
 * 
 */
void ChatBrowser::msgDisconnect()
{
  append(tr("<div class='np'>(%1) <i class='err'>Соединение разорвано</i></div>").arg(currentTime()));
}


/** [public]
 * Уведомление о новом участнике `const QString &nick`,
 * используются различные сообщения в зависимости от пола `quint8 sex`.
 */
void ChatBrowser::msgNewParticipant(quint8 sex, const QString &nick)
{
  if (sex)
    append(tr("<div class='np'>(%1) <i><b>%2</b> зашла в чат</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  else
    append(tr("<div class='np'>(%1) <i><b>%2</b> зашёл в чат</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  
  scroll();
}


/** [public]
 * Уведомление о выходе участнике `const QString &nick`,
 * используются различные сообщения в зависимости от пола `quint8 sex`.
 */
void ChatBrowser::msgParticipantLeft(quint8 sex, const QString &nick)
{
  if (sex)
    append(tr("<div class='np'>(%1) <i><b>%2</b> вышла из чата</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  else
    append(tr("<div class='np'>(%1) <i><b>%2</b> вышел из чата</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  
  scroll();
}


/** [public]
 * 
 */
void ChatBrowser::msgReadyForUse(const QString &s)
{
  append(tr("<div class='np'>(%1) <i class='green'>Успешно подключены к %2</i></div>").arg(currentTime()).arg(s));
}


/** [protected]
 * 
 */
void ChatBrowser::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu *menu = createStandardContextMenu();
  menu->exec(event->globalPos());
  delete menu;
}


/** [public slots]
 * Новое сообщение `const QString &message`,
 * от участника `const QString &nick`.
 */
void ChatBrowser::msgNewMessage(const QString &nick, const QString &message)
{
  append(tr("<div><small class='gr'>(%1) <b>%2</b>:</small> %3</div>")
      .arg(currentTime())
      .arg(Qt::escape(nick))
      .arg(Qt::escape(message)));
  
  scroll();
}


/** [private]
 * Принудительно скролит текст
 */
void ChatBrowser::scroll()
{
  QScrollBar *bar = verticalScrollBar();
  bar->setValue(bar->maximum());
}
