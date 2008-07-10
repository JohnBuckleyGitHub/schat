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
  m_channelLog = new ChannelLog(this);
  setFocusPolicy(Qt::NoFocus);
  setOpenExternalLinks(true);
  document()->setDefaultStyleSheet(
      ".gr    { color:#90a4b3; }"
      ".green { color:#6bb521; }"
      ".err   { color:#da251d; }"
      ".info  { color:#5096cf; }"
      ".me    { color:#f3f; }");
}


/** [public]
 * 
 */
void ChatBrowser::msg(const QString &text)
{
  m_channelLog->msg(text);
  append(tr("<div><small class='gr'>(%1)</small> %2</div>").arg(currentTime()).arg(text));
  scroll();
}


/** [public]
 * 
 */
void ChatBrowser::msgChangedNick(quint16 sex, const QString &oldNick, const QString &newNick)
{
  if (sex)
    msg(tr("<i class='info'><b>%1</b> теперь известна как <b>%2</b></i>").arg(Qt::escape(oldNick)).arg(Qt::escape(newNick)));
  else
    msg(tr("<i class='info'><b>%1</b> теперь известен как <b>%2</b></i>").arg(Qt::escape(oldNick)).arg(Qt::escape(newNick)));
}


/** [public]
 * 
 */
void ChatBrowser::msgChangedProfile(quint16 sex, const QString &nick)
{
  if (sex)
    msg(tr("<i class='gr'><b>%1</b> изменила свой профиль</i>").arg(Qt::escape(nick)));
  else
    msg(tr("<i class='gr'><b>%1</b> изменил свой профиль</i>").arg(Qt::escape(nick)));
}


/** [public]
 * 
 */
void ChatBrowser::msgDisconnect()
{
  msg(tr("<i class='err'>Соединение разорвано</i>"));
}


/** [public]
 * Уведомление о новом участнике `const QString &nick`,
 * используются различные сообщения в зависимости от пола `quint8 sex`.
 */
void ChatBrowser::msgNewParticipant(quint8 sex, const QString &nick)
{
  if (sex)
    msg(tr("<i class='gr'><b>%1</b> зашла в чат</i>").arg(Qt::escape(nick)));
  else
    msg(tr("<i class='gr'><b>%1</b> зашёл в чат</i>").arg(Qt::escape(nick)));
}


/** [public]
 * Уведомление о выходе участнике `const QString &nick`,
 * используются различные сообщения в зависимости от пола `quint8 sex`.
 */
void ChatBrowser::msgParticipantLeft(quint8 sex, const QString &nick, const QString &bye)
{
  QString byeMsg;
  if (!bye.isEmpty())
    byeMsg = ": <span style='color:#909090;'>" + Qt::escape(bye) + "</span>";
  
  if (sex)
    msg(tr("<i class='gr'><b>%1</b> вышла из чата%2</i>").arg(Qt::escape(nick)).arg(byeMsg));
  else
    msg(tr("<i class='gr'><b>%1</b> вышел из чата%2</i>").arg(Qt::escape(nick)).arg(byeMsg));
}


/** [public]
 * 
 */
void ChatBrowser::msgReadyForUse(const QString &addr)
{
  msg(tr("<i class='green'>Успешно подключены к %1</i>").arg(addr));
}


/** [public]
 * 
 */
void ChatBrowser::msgReadyForUse(const QString &network, const QString &addr)
{
  msg(tr("<i class='green'>Успешно подключены к сети <b>%1</b> (%2)</i>").arg(network).arg(addr));
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
  if (message.startsWith("/me ", Qt::CaseInsensitive))
    msg(tr("<span class='me'>%1 %2</span>").arg(nick).arg(message.mid(message.indexOf(QChar(' ')))));
  else
    msg(tr("<b class='gr'>%1:</b> %2").arg(Qt::escape(nick)).arg(Qt::escape(message)));
}
