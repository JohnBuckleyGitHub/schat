/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui>
#include <QtWebKit>

#include "channellog.h"
#include "chatview.h"
#include "chatview_p.h"
#include "settings.h"

#ifndef SCHAT_NO_WEBKIT
  #include "chatwindowstyle.h"
  #include "chatwindowstyleoutput.h"
#endif

/*!
 * \brief Конструктор класса ChatViewPrivate.
 */
ChatViewPrivate::ChatViewPrivate(ChatView *parent)
  : empty(true), q(parent)
  #ifndef SCHAT_NO_WEBKIT
  , style(0)
  #endif
{
  #ifdef SCHAT_NO_WEBKIT
  styleSheet = "a { color:#815d53; text-decoration:none; }"
    ".sender, .sender a { color:#185074; }"
    ".me, .meSender a { color:#cd00cd; }"
    ".oldClientProtocol, .oldServerProtocol, .badNickName, .accessDenied, .disconnect { color:#da251d; }"
    ".ts, .preSb, .newUser, .newUser a, .userLeft, .userLeft a { color:#8797a3; }"
    ".newUser a, .userLeft a, .meSender a { font-weight:bold; }"
    ".ready { color:#6bb521; }"
    ".info, .changedNick, .changedNick a { color:#5096cf; }"
    ".changedNick a { font-weight:bold; }";
  #endif
}


ChatViewPrivate::~ChatViewPrivate()
{
  #ifndef SCHAT_NO_WEBKIT
    if (style)
      delete style;
  #endif
}


/*!
 * Проверяет входящее сообщение на наличие команды и при
 * необходимости вырезает эту команду из строки.
 *
 * \param cmd Команда.
 * \param msg Сообщение.
 * \param cut Разрешает вырезание команды из строки.
 *
 * \return Возвращает \a true если команда была найдена.
 */
bool ChatViewPrivate::prepareCmd(const QString &cmd, QString &msg, bool cut)
{
  if (ChannelLog::toPlainText(msg).startsWith(cmd, Qt::CaseInsensitive)) {
    if (cut) {
      int index = msg.indexOf(cmd, 0, Qt::CaseInsensitive);
      if (index != -1)
        msg.remove(index, cmd.size());
    }
    return true;
  }

  return false;
}


/*!
 * Записывает строку в журнал.
 */
void ChatViewPrivate::toLog(const QString &text)
{
  if (log) {
    if (!channelLog) {
      channelLog = new ChannelLog(q);
      channelLog->setChannel(channel);
    }
    channelLog->msg(text);
  }
}


#ifdef SCHAT_NO_WEBKIT
QString ChatViewPrivate::makeMessage(const QString &sender, const QString &message, bool action)
{
  if (action)
    return makeStatus(QString("<span class='meSender'>%1</span> <span class='me'>%2</span>").arg(sender).arg(message));
  else
    return makeStatus(QString("<b class='sender'>%1:</b> %2").arg(sender).arg(message));
}


QString ChatViewPrivate::makeStatus(const QString &message)
{
  return QString("<div><small class='ts'>%1 |</small> %2</div>").arg(QTime::currentTime().toString("hh:mm:ss")).arg(message);
}
#endif


/*!
 * \brief Конструктор класса ChatView.
 */
ChatView::ChatView(QWidget *parent)
#ifndef SCHAT_NO_WEBKIT
  : QWebView(parent)
#else
  : QTextBrowser(parent)
#endif
{
  d = new ChatViewPrivate(this);

  #ifndef SCHAT_NO_WEBKIT
    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    d->style = new ChatWindowStyleOutput("Default", "");
    setHtml(d->style->makeSkeleton());
    connect(this, SIGNAL(linkClicked(const QUrl &)), SLOT(linkClicked(const QUrl &)));
  #else
    setOpenLinks(false);
    document()->setDefaultStyleSheet(d->styleSheet);
    setFrameShape(QFrame::NoFrame);
    connect(this, SIGNAL(anchorClicked(const QUrl &)), SLOT(linkClicked(const QUrl &)));
    #if QT_VERSION >= 0x040500
      document()->setDocumentMargin(2);
    #endif
  #endif

  connect(settings, SIGNAL(changed(int)), SLOT(notify(int)));
  setFocusPolicy(Qt::NoFocus);

  d->strict = Emoticons::strictParse();
  createActions();
}


ChatView::~ChatView()
{
  delete d;
}


QString ChatView::channel() const
{
  return d->channel;
}


/*!
 * Уведомление о смене пользователем ника.
 */
QString ChatView::statusChangedNick(quint8 gender, const QString &oldNick, const QString &newNick)
{
  QString nick    = Qt::escape(oldNick);
  QString nickHex = newNick.toUtf8().toHex();
  QString nickNew = Qt::escape(newNick);
  QString html = "<span class='changedNick'>";

  if (gender)
    html += tr("<b>%1</b> теперь известна как <a href='nick:%2'>%3</a>").arg(nick).arg(nickHex).arg(nickNew);
  else
    html += tr("<b>%1</b> теперь известен как <a href='nick:%2'>%3</a>").arg(nick).arg(nickHex).arg(nickNew);

  html += "</span>";
  return html;
}


/*!
 * Уведомление об подключении нового пользователя.
 */
QString ChatView::statusNewUser(quint8 gender, const QString &nick)
{
  QString escaped = Qt::escape(nick);
  QString nickHex = nick.toUtf8().toHex();
  QString out = "<span class='newUser'>";

  if (gender)
    out += tr("<a href='nick:%1'>%2</a> зашла в чат").arg(nickHex).arg(escaped);
  else
    out += tr("<a href='nick:%1'>%2</a> зашёл в чат").arg(nickHex).arg(escaped);

  out += "</span>";
  return out;
}


/*!
 * Уведомление об отключении пользователя.
 */
QString ChatView::statusUserLeft(quint8 gender, const QString &nick, const QString &bye)
{
  QString escaped = Qt::escape(nick);
  QString nickHex = nick.toUtf8().toHex();
  QString out = "<span class='userLeft'>";

  QString byeMsg;
  if (!bye.isEmpty())
    byeMsg = ": <span style='color:#909090;'>" + Qt::escape(bye) + "</span>";

  if (gender)
    out += tr("<a href='nick:%1'>%2</a> вышла из чата%3").arg(nickHex).arg(escaped).arg(byeMsg);
  else
    out += tr("<a href='nick:%1'>%2</a> вышел из чата%3").arg(nickHex).arg(escaped).arg(byeMsg);

  out += "</span>";
  return out;
}


/*!
 * Универсальное фильтрованное сервисное сообщение.
 */
void ChatView::addFilteredMsg(const QString &msg, bool strict)
{
  QTextDocument doc;
  #ifdef SCHAT_NO_WEBKIT
    doc.setDefaultStyleSheet(d->styleSheet);
  #endif
  doc.setHtml(msg);
  QString html = ChannelLog::htmlFilter(doc.toHtml(), 0, strict);
  html = QString("<span class='preSb'>%1</span><div class='sb'>%2</div>").arg(tr("Сервисное сообщение:")).arg(html);

  addServiceMsg(html);
}


/*!
 * Добавление сообщения от пользователя.
 */
void ChatView::addMsg(const QString &sender, const QString &message, bool direction)
{
  d->empty = false;

  QTextDocument doc;
  #ifdef SCHAT_NO_WEBKIT
    doc.setDefaultStyleSheet(d->styleSheet);
  #endif
  doc.setHtml(message);
  QString html = ChannelLog::htmlFilter(doc.toHtml());
  html = ChannelLog::parseLinks(html);

  QString escapedNick = Qt::escape(sender);

  bool action = false;
  bool same = false;

  if (d->prepareCmd("/me ", html)) {
    action = true;
    d->prev = "";

    d->toLog(QString("<span class='me'>%1 %2</span>").arg(escapedNick).arg(html));
  }
  else {
    if (d->prev.isEmpty() || d->prev != sender)
      d->prev = sender;
    else
      same = true;

    d->toLog(QString("<b class='sender'>%1:</b> %2").arg(escapedNick).arg(html));
  }

  if (settings->emoticons()) {
    EmoticonsTheme::ParseMode mode = EmoticonsTheme::StrictParse;
    if (!d->strict)
      mode = EmoticonsTheme::RelaxedParse;

    html = settings->emoticons()->theme().parseEmoticons(html, mode);
  }

  QString name = "<a href='nick:" + sender.toUtf8().toHex() + "'>" + escapedNick + "</a>";
  #ifndef SCHAT_NO_WEBKIT
    appendMessage(d->style->makeMessage(name, html, direction, same, "", action), same);
  #else
    appendMessage(d->makeMessage(name, html, action));
  #endif

//  qDebug() << m_view->page()->mainFrame()->toHtml();
}


/*!
 * Универсальное сервисное сообщение.
 */
void ChatView::addServiceMsg(const QString &msg)
{
  d->empty = false;
  d->prev = "";

  d->toLog(msg);
  #ifndef SCHAT_NO_WEBKIT
    appendMessage(d->style->makeStatus(msg));
  #else
    appendMessage(d->makeStatus(msg));
  #endif
}


void ChatView::channel(const QString &ch)
{
  d->channel = ch;

  if (d->channelLog)
    d->channelLog->setChannel(ch);
}


void ChatView::log(bool enable)
{
  d->log = enable;

  if (enable) {
    if (!d->channelLog) {
      d->channelLog = new ChannelLog(this);
      d->channelLog->setChannel(d->channel);
    }
  }
  else if (d->channelLog)
    d->channelLog->deleteLater();
}


void ChatView::scroll()
{
  #ifndef SCHAT_NO_WEBKIT
    page()->mainFrame()->evaluateJavaScript("alignChat(true)");
  #else
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
  #endif
}


/*!
 * Возвращает \a true в случае успешного копирования.
 */
bool ChatView::copy()
{
  #ifndef SCHAT_NO_WEBKIT
    if (selectedText().isEmpty())
      return false;

    triggerPageAction(QWebPage::Copy);
  #else
    if (!textCursor().hasSelection())
      return false;

    QTextBrowser::copy();
  #endif

  return true;
}


/*!
 * Очистка окна чата.
 */
void ChatView::clear()
{
  #ifndef SCHAT_NO_WEBKIT
    setHtml(d->style->makeSkeleton());
  #else
    QTextBrowser::clear();
  #endif

  d->empty = true;
  d->prev = "";
}


void ChatView::contextMenuEvent(QContextMenuEvent *event)
{
  #ifndef SCHAT_NO_WEBKIT
    d->copy->setEnabled(!selectedText().isEmpty());
  #else
    d->copy->setEnabled(textCursor().hasSelection());
    d->selectAll->setEnabled(!d->empty);
  #endif

  QString copyLinkText = tr("Копировать &ссылку");
  d->clear->setEnabled(!d->empty);
  QAction *copyLink = 0;

  QMenu menu(this);
  menu.addAction(d->copy);

  #ifndef SCHAT_NO_WEBKIT
    QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());
    QUrl url = r.linkUrl();
    if (!url.isEmpty() && url.scheme() != "nick"  && url.scheme() != "smile") {
      copyLink = pageAction(QWebPage::CopyLinkToClipboard);
      copyLink->setText(copyLinkText);
    }
  #else
    QUrl url(anchorAt(event->pos()));
    if (!url.isEmpty() && url.scheme() != "nick" && url.scheme() != "smile")
      copyLink = menu.addAction(copyLinkText);
  #endif

  if (copyLink)
    menu.addAction(copyLink);

  menu.addSeparator();
  menu.addAction(d->clear);

  #ifndef SCHAT_NO_WEBKIT
    menu.exec(event->globalPos());
  #else
    menu.addAction(d->selectAll);
    QAction *action = menu.exec(event->globalPos());

    if (action == copyLink)
      QApplication::clipboard()->setText(url.toString());
  #endif
}


void ChatView::linkClicked(const QUrl &url)
{
  QString scheme = url.scheme();

  if (scheme == "nick") {
    emit nickClicked(QByteArray::fromHex(url.toString(QUrl::RemoveScheme).toLatin1()));
  }
  else if (scheme == "smile") {
    emit emoticonsClicked("&nbsp;" + QByteArray::fromHex(url.toString(QUrl::RemoveScheme).toLatin1()) + "&nbsp;");
  }
  else
    QDesktopServices::openUrl(url);
}


void ChatView::notify(int notify)
{
  if (notify == Settings::EmoticonsChanged)
    d->strict = Emoticons::strictParse();
}


void ChatView::appendMessage(const QString &message, bool same_from)
{
  #ifndef SCHAT_NO_WEBKIT
    QString js_message = message;
    js_message.replace("\"","\\\"");
    js_message.replace("\n","\\n");
    js_message = QString("append%2Message(\"%1\");").arg(js_message).arg(same_from ? "Next" : "");
    page()->mainFrame()->evaluateJavaScript(js_message);
  #else
    append(message);
    scroll();
  #endif
}


void ChatView::createActions()
{
  d->copy = new QAction(QIcon(":/images/editcopy.png"), tr("&Копировать"), this);
  d->copy->setShortcut(Qt::CTRL + Qt::Key_C);
  connect(d->copy, SIGNAL(triggered()), SLOT(copy()));

  d->clear = new QAction(QIcon(":/images/editclear.png"), tr("&Очистить"), this);
  connect(d->clear, SIGNAL(triggered()), SLOT(clear()));

  #ifdef SCHAT_NO_WEBKIT
    d->selectAll = new QAction(tr("&Выделить всё"), this);
    connect(d->selectAll, SIGNAL(triggered()), SLOT(selectAll()));
  #endif
}
