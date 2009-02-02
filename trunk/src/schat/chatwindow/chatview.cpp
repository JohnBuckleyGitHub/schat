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
#include "chatwindowstyle.h"
#include "chatwindowstyleoutput.h"
#include "settings.h"


/*!
 * \brief Конструктор класса ChatViewPrivate.
 */
ChatViewPrivate::ChatViewPrivate(ChatView *parent)
  : empty(true), q(parent), style(0)
{
}


ChatViewPrivate::~ChatViewPrivate()
{
  if (style)
    delete style;
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


/*!
 * \brief Конструктор класса ChatView.
 */
ChatView::ChatView(QWidget *parent)
  : QWebView(parent)
{
  d = new ChatViewPrivate(this);

  page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  d->style = new ChatWindowStyleOutput("Default", "");
  setHtml(d->style->makeSkeleton());
  setFocusPolicy(Qt::NoFocus);

  connect(this, SIGNAL(linkClicked(const QUrl &)), SLOT(linkClicked(const QUrl &)));
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
  d->empty = false;
  d->prev = "";

  QTextDocument doc;
  doc.setHtml(msg);
  QString html = ChannelLog::htmlFilter(doc.toHtml(), 0, strict);
  html = tr("<span class='preSb'>Сервисное сообщение:</span>") + "<div class='sb'>" + html + "</div>";

  d->toLog(html);
  appendMessage(d->style->makeStatus(html));
}


/*!
 * Добавление сообщения от пользователя.
 */
void ChatView::addMsg(const QString &sender, const QString &message, bool direction)
{
  d->empty = false;

  QTextDocument doc;
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
  appendMessage(d->style->makeMessage(name, html, direction, same, "", action), same);

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
  appendMessage(d->style->makeStatus(msg));
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
  page()->mainFrame()->evaluateJavaScript("alignChat(true)");
}


/*!
 * Возвращает \a true в случае успешного копирования.
 */
bool ChatView::copy()
{
  if (selectedText().isEmpty())
    return false;

  triggerPageAction(QWebPage::Copy);
  return true;
}


/*!
 * Очистка окна чата.
 */
void ChatView::clear()
{
  setHtml(d->style->makeSkeleton());
  d->empty = true;
  d->prev = "";
}


void ChatView::contextMenuEvent(QContextMenuEvent *event)
{
  d->copy->setEnabled(!selectedText().isEmpty());
  d->clear->setEnabled(!d->empty);

  QMenu menu(this);
  menu.addAction(d->copy);

  QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());
  QUrl url = r.linkUrl();
  if (!url.isEmpty() && url.scheme() != "nick"  && url.scheme() != "smile")
    menu.addAction(pageAction(QWebPage::CopyLinkToClipboard));

  menu.addSeparator();
  menu.addAction(d->clear);

  menu.exec(event->globalPos());
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


void ChatView::appendMessage(QString message, bool same_from)
{
  QString js_message = QString("append%2Message(\"%1\");").arg(message.replace("\"","\\\"").replace("\n","\\n")).arg(same_from?"Next":"");
  page()->mainFrame()->evaluateJavaScript(js_message);
}


void ChatView::createActions()
{
  d->copy = new QAction(QIcon(":/images/editcopy.png"), tr("&Копировать"), this);
  d->copy->setShortcut(Qt::CTRL + Qt::Key_C);
  connect(d->copy, SIGNAL(triggered()), SLOT(copy()));

  d->clear = new QAction(QIcon(":/images/editclear.png"), tr("&Очистить"), this);
  connect(d->clear, SIGNAL(triggered()), SLOT(clear()));
}
