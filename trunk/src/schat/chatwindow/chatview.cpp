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
#include "chatwindowstyle.h"
#include "chatwindowstyleoutput.h"
#include "settings.h"

class ChatView::ChatViewPrivate
{
public:
  ChatViewPrivate();
  ~ChatViewPrivate();
  bool log;
  bool strict;
  ChatWindowStyleOutput *style;
  QPointer<ChannelLog> channelLog;
  QString channel;
  QString prev;
};


ChatView::ChatViewPrivate::ChatViewPrivate()
  : style(0)
{
}


ChatView::ChatViewPrivate::~ChatViewPrivate()
{
  if (style)
    delete style;
}


/*!
 * \brief Конструктор класса ChatView.
 */
ChatView::ChatView(QWidget *parent)
  : QWidget(parent)
{
  d = new ChatViewPrivate;

  m_view = new QWebView(this);
  m_view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  d->style = new ChatWindowStyleOutput("Default", "");
  m_view->setHtml(d->style->makeSkeleton());

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(m_view);
  mainLay->setMargin(0);

  connect(m_view, SIGNAL(linkClicked(const QUrl &)), SLOT(linkClicked(const QUrl &)));
  connect(settings, SIGNAL(changed(int)), SLOT(notify(int)));
  setFocusPolicy(Qt::NoFocus);

  d->strict = Emoticons::strictParse();
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
  QString html = "<span class='statusChangedNick'>";

  if (gender)
    html += tr("<b>%1</b> теперь известна как <b><a href='nick:%2'>%3</a></b>").arg(nick).arg(nickHex).arg(nickNew);
  else
    html += tr("<b>%1</b> теперь известен как <b><a href='nick:%2'>%3</a></b>").arg(nick).arg(nickHex).arg(nickNew);

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
  QString out = "<span class='statusNewUser'>";

  if (gender)
    out += tr("<b><a href='nick:%1'>%2</a></b> зашла в чат").arg(nickHex).arg(escaped);
  else
    out += tr("<b><a href='nick:%1'>%2</a></b> зашёл в чат").arg(nickHex).arg(escaped);

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
  QString out = "<span class='statusUserLeft'>";

  QString byeMsg;
  if (!bye.isEmpty())
    byeMsg = ": <span style='color:#909090;'>" + Qt::escape(bye) + "</span>";

  if (gender)
    out += tr("<b><a href='nick:%1' class='gr'>%2</a></b> вышла из чата%3").arg(nickHex).arg(escaped).arg(byeMsg);
  else
    out += tr("<b><a href='nick:%1' class='gr'>%2</a></b> вышел из чата%3").arg(nickHex).arg(escaped).arg(byeMsg);

  out += "</span>";
  return out;
}


/*!
 * Универсальное фильтрованное сервисное сообщение.
 */
void ChatView::addFilteredMsg(const QString &msg, bool strict)
{
  QTextDocument doc;
  doc.setHtml(msg);
  QString html = ChannelLog::htmlFilter(doc.toHtml(), 0, strict);
//  qDebug() << html;
  toLog(html);
  appendMessage(d->style->makeStatus(html));
}


/*!
 * Добавление сообщения от пользователя.
 */
void ChatView::addMsg(const QString &sender, const QString &message, bool direction)
{
  QTextDocument doc;
  doc.setHtml(message);
  QString html = ChannelLog::htmlFilter(doc.toHtml());
  html = ChannelLog::parseLinks(html);

  if (settings->emoticons()) {
    EmoticonsTheme::ParseMode mode = EmoticonsTheme::StrictParse;
    if (!d->strict)
      mode = EmoticonsTheme::RelaxedParse;

    qDebug() << d->strict;

    html = settings->emoticons()->theme().parseEmoticons(html, mode);
  }

//  qDebug() << html;
  QString escapedNick = Qt::escape(sender);

  bool action = false;
  bool same = false;

  if (prepareCmd("/me ", html)) {
    action = true;
    d->prev = "";

    toLog(QString("<span class='me'>%1 %2</span>").arg(escapedNick).arg(html));
  }
  else {
    if (d->prev.isEmpty() || d->prev != sender)
      d->prev = sender;
    else
      same = true;

    toLog(QString("<b class='gr'>%1:</b> %2").arg(escapedNick).arg(html));
  }

  appendMessage(d->style->makeMessage(sender, html, direction, same, "", action), same);
}


/*!
 * Универсальное сервисное сообщение.
 */
void ChatView::addServiceMsg(const QString &msg)
{
  toLog(msg);
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


void ChatView::linkClicked(const QUrl &url)
{
//  qDebug() << "ChatView::linkClicked()" << url.toString();

  QDesktopServices::openUrl(url);
}


void ChatView::notify(int notify)
{
  if (notify == Settings::EmoticonsChanged)
    d->strict = Emoticons::strictParse();
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
bool ChatView::prepareCmd(const QString &cmd, QString &msg, bool cut) const
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


void ChatView::appendMessage(QString message, bool same_from)
{
//  qDebug() << "ChatView::appendMessage()" << message << same_from;

  QString js_message = QString("append%2Message(\"%1\");").arg(message.replace("\"","\\\"").replace("\n","\\n")).arg(same_from?"Next":"");
  m_view->page()->mainFrame()->evaluateJavaScript(js_message);
}


/*!
 * Записывает строку в журнал.
 */
void ChatView::toLog(const QString &text)
{
  if (d->log) {
    if (!d->channelLog) {
      d->channelLog = new ChannelLog(this);
      d->channelLog->setChannel(d->channel);
    }
    d->channelLog->msg(text);
  }
}
