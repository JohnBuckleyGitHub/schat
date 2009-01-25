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

#include "chatview.h"
#include "chatwindowstyle.h"
#include "chatwindowstyleoutput.h"

/*!
 * \brief Конструктор класса ChatView.
 */
ChatView::ChatView(QWidget *parent)
  : QWidget(parent)
{
  m_view = new QWebView(this);
  m_view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  m_style = new ChatWindowStyleOutput("Default", "");
  m_view->setHtml(m_style->makeSkeleton());

  QHBoxLayout *mainLay = new QHBoxLayout(this);
  mainLay->addWidget(m_view);
  mainLay->setMargin(0);

  connect(m_view, SIGNAL(linkClicked(const QUrl &)), SLOT(linkClicked(const QUrl &)));
  setFocusPolicy(Qt::NoFocus);
}


ChatView::~ChatView()
{
  delete m_style;
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


void ChatView::addMsg(const QString &sender, const QString &message, bool direction)
{
  bool same = false;

  if (m_prev.isEmpty() || m_prev != sender)
    m_prev = sender;
  else
    same = true;

  appendMessage(m_style->makeMessage(sender, message, direction, same), same);
}


/*!
 * Универсальное сервисное сообщение.
 */
void ChatView::addServiceMsg(const QString &msg)
{
  toLog(msg);
  appendMessage(m_style->makeStatus(msg));
}


void ChatView::log(bool enable)
{
  m_log = enable;

  if (enable) {
    if (!m_channelLog) {
      m_channelLog = new ChannelLog(this);
      m_channelLog->setChannel(m_channel);
    }
  }
  else if (m_channelLog)
    m_channelLog->deleteLater();
}


void ChatView::linkClicked(const QUrl &url)
{
//  qDebug() << "ChatView::linkClicked()" << url.toString();

  QDesktopServices::openUrl(url);

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
  if (m_log) {
    if (!m_channelLog) {
      m_channelLog = new ChannelLog(this);
      m_channelLog->setChannel(m_channel);
    }
    m_channelLog->msg(text);
  }
}
