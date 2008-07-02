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
      ".gr    { color:#909090; }"
      ".green { color:#6bb521; }"
      ".err   { color:#da251d; }"
      ".info  { color:#5096cf; }"
      ".me    { color:#f3f; }");
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
void ChatBrowser::msg(const QString &text)
{
  append(tr("<div><small class='gr'>(%1)</small> %2</div>").arg(currentTime()).arg(text));
  scroll();
}


/** [public]
 * 
 */
void ChatBrowser::msgChangedNick(quint16 sex, const QString &oldNick, const QString &newNick)
{
  if (sex)
    append(tr("<div><small class='gr'>(%1)</small> <i class='info'><b>%2</b> теперь известна как <b>%3</b></i></div>").arg(currentTime()).arg(Qt::escape(oldNick)).arg(Qt::escape(newNick)));
  else
    append(tr("<div><small class='gr'>(%1)</small> <i class='info'><b>%2</b> теперь известен как <b>%3</b></i></div>").arg(currentTime()).arg(Qt::escape(oldNick)).arg(Qt::escape(newNick)));
  
  scroll(); 
}


/** [public]
 * 
 */
void ChatBrowser::msgChangedProfile(quint16 sex, const QString &nick)
{
  if (sex)
    append(tr("<div class='gr'><small>(%1)</small> <i><b>%2</b> изменила свой профиль</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  else
    append(tr("<div class='gr'><small>(%1)</small> <i><b>%2</b> изменил свой профиль</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  
  scroll();  
}


/** [public]
 * 
 */
void ChatBrowser::msgDisconnect()
{
  append(tr("<div><small class='gr'>(%1)</small> <i class='err'>Соединение разорвано</i></div>").arg(currentTime()));
}


/** [public]
 * Уведомление о новом участнике `const QString &nick`,
 * используются различные сообщения в зависимости от пола `quint8 sex`.
 */
void ChatBrowser::msgNewParticipant(quint8 sex, const QString &nick)
{
  if (sex)
    append(tr("<div class='gr'><small>(%1)</small> <i><b>%2</b> зашла в чат</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  else
    append(tr("<div class='gr'><small>(%1)</small> <i><b>%2</b> зашёл в чат</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  
  scroll();
}


/** [public]
 * Уведомление о выходе участнике `const QString &nick`,
 * используются различные сообщения в зависимости от пола `quint8 sex`.
 */
void ChatBrowser::msgParticipantLeft(quint8 sex, const QString &nick)
{
  if (sex)
    append(tr("<div class='gr'><small>(%1)</small> <i><b>%2</b> вышла из чата</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  else
    append(tr("<div class='gr'><small>(%1)</small> <i><b>%2</b> вышел из чата</i></div>").arg(currentTime()).arg(Qt::escape(nick)));
  
  scroll();
}


/** [public]
 * 
 */
void ChatBrowser::msgReadyForUse(const QString &addr)
{
  append(tr("<div><small class='gr'>(%1)</small> <i class='green'>Успешно подключены к %2</i></div>").arg(currentTime()).arg(addr));
}


/** [public]
 * 
 */
void ChatBrowser::msgReadyForUse(const QString &network, const QString &addr)
{
  append(tr("<div><small class='gr'>(%1)</small> <i class='green'>Успешно подключены к сети <b>%2</b> (%3)</i></div>").arg(currentTime()).arg(network).arg(addr));
}

/** [public]
 * Принудительно скролит текст
 */
void ChatBrowser::scroll()
{
  QScrollBar *bar = verticalScrollBar();
  bar->setValue(bar->maximum());
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
  append(tr("<div><small class='gr'>(%1)</small> <b class='gr'>%2:</b> %3</div>")
      .arg(currentTime())
      .arg(Qt::escape(nick))
      .arg(Qt::escape(message)));
  
  scroll();
}
