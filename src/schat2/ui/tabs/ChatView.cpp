/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#include <QFile>
#include <QWebFrame>

#include "debugstream.h"
#include "ChatMessage.h"
#include "ChatView.h"
#include "ChatView_p.h"


QString ChatViewPrivate::tpl(Templates tplName)
{
  if (m_templates.contains(tplName))
    return m_templates.value(tplName);

  QString fileName = ":/html/templates/";
  if (tplName == UserMessage)
    fileName += "message.html";
  else
    return "";

  QFile file(fileName);
  if (file.open(QIODevice::ReadOnly)) {
    QString tpl = file.readAll();
    m_templates.insert(tplName, tpl);
    return tpl;
  }

  return "";
}

ChatView::ChatView(QWidget *parent)
  : QWebView(parent)
  , m_loaded(false)
  , m_d(new ChatViewPrivate())
{
  setAcceptDrops(false);
  QWebSettings::globalSettings()->setFontSize(QWebSettings::DefaultFontSize, fontInfo().pixelSize());
  QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

  setUrl(QUrl("qrc:/html/ChatView.html"));
  connect(this, SIGNAL(loadFinished(bool)), SLOT(loadFinished()));
  connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(populateJavaScriptWindowObject()));

}


void ChatView::append(const ChatMessage &message)
{
  if (message.type() == ChatMessage::UserMessage)
    appendUserMessage(message);
}


/*!
 * Базовая функция добавления сообщения.
 * \todo Сделать эту функцию приватной.
 */
void ChatView::appendRawMessage(const QString &message)
{
  QString js = message;
  js.replace("\"","\\\"");
  js.replace("\n","\\n");
  js = QString("appendMessage(\"%1\");").arg(js);

  if (m_loaded) {
    page()->mainFrame()->evaluateJavaScript(js);
  }
  else
    m_pendingJs.enqueue(js);
}


/*!
 * Обработка одиночного клика по нику.
 * \todo ! Реализовать обработку двойного клика для открытия привата.
 */
void ChatView::nickClicked(const QString &arg1)
{
  QByteArray id = userIdFromClass(arg1);
  if (id.isEmpty())
    return;

  emit nickClicked(id);
}


/*!
 * Завершение загрузки документа.
 */
void ChatView::loadFinished()
{
  qDebug() << "loadFinished()";
  m_loaded = true;

  while (!m_pendingJs.isEmpty())
    page()->mainFrame()->evaluateJavaScript(m_pendingJs.dequeue());
}


void ChatView::populateJavaScriptWindowObject()
{
  page()->mainFrame()->addToJavaScriptWindowObject("ChatView", this);
}


/*!
 * Получение идентификатора пользователя из CSS класса.
 */
QByteArray ChatView::userIdFromClass(const QString &text)
{
  if (text.size() < 47 || !text.startsWith("nick "))
    return QByteArray();

  return QByteArray::fromHex(text.mid(5, 42).toLatin1());
}


void ChatView::appendUserMessage(const ChatMessage &message)
{
  SCHAT_DEBUG_STREAM(this << "appendUserMessage()" << message.nick() << message.text());

  if (message.text().isEmpty())
    return;

  QString extra;
  if (message.status() & ChatMessage::IncomingMessage)
    extra += " incoming";

  if (message.status() & ChatMessage::OutgoingMessage)
    extra += " outgoing";

  QString html = m_d->tpl(ChatViewPrivate::UserMessage);
  setTimeStamp(html);
  setText(html, message.text());

  html.replace("%nick%", message.nick());
  html.replace("%extra%", extra);
  html.replace("%userId%", message.senderId().toHex());
  html.replace("%messageId%", message.messageId());

  appendRawMessage(html);
}


void ChatView::setText(QString &html, const QString &text)
{
  QString msg = text;
  html.replace("%message%", msg.replace("\\","\\\\").remove('\r').replace("%","&#37;"));
}


void ChatView::setTimeStamp(QString &html)
{
  QDateTime dateTime = QDateTime::currentDateTime();
  html.replace("%time%", dateTime.toString("hh:mm"));
  html.replace("%seconds%", dateTime.toString(":ss"));
}
