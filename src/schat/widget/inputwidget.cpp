/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui>

#include "widget/inputwidget.h"

/*!
 * \class InputWidget
 * \brief Базовый виджет для ввода текста.
 */

/*!
 * \brief Конструктор класса InputWidget.
 */
InputWidget::InputWidget(QWidget *parent)
  : QTextEdit(parent)
{
  QFontInfo fontInfo(currentFont());
  setMinimumHeight(fontInfo.pixelSize() * 2);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_default = currentCharFormat();
  m_current = 0;
  document()->setDefaultStyleSheet("a {color:#1a4d82;}");
}


void InputWidget::sendMsg()
{
//  processLinks();

  QString html = toHtml();
  // Очистка полного html документа от ненужных тегов.
  QRegExp badStuff ("<![^<>]*>|<head[^<>]*>.*</head[^<>]*>|</?html[^<>]*>|</?body[^<>]*>|</?p[^<>]*>");
  html.remove(badStuff);
  html = html.trimmed();

  if (html.isEmpty())
    return;

  html = html.left(8192);
  html.replace(QChar('\n'), "<br />");

  while (html.contains("<br /><br />"))
    html.replace("<br /><br />", "<br />");

  html.replace("<br /></span>", "</span>");

  if (html.endsWith("<br />"))
    html.left(html.size() - 6);

  m_msg << html;
  m_current = m_msg.count();

  html = parseLinks(html);

  emit sendMsg(html);

//  qDebug() << html;
}


/*!
 * Очистка документа.
 */
void InputWidget::clearMsg()
{
  clear();
  setCurrentCharFormat(m_default);
}


/*!
 * \brief Обработка событий нажатия клавиш.
 * 
 * Если нажата кнопка \b Return производится формирование строки для отправки.
 *  - Вырезаются лишние части html документа.
 *  - Если в результате осталась пустая строка, то выход.
 *  - Сообщение обрезается до 8192 символов.
 *  - Удаляются все двойные переносы строк.
 */
void InputWidget::keyPressEvent(QKeyEvent *event)
{
  QKeySequence seq = event->key() + event->modifiers();
  QString key = seq.toString();

  if (key == "Return")
    sendMsg();
  else if (key == "Ctrl+Up")
    nextMsg();
  else if (key == "Ctrl+Down")
    prevMsg();
  else if (event->key() == Qt::Key_Tab)
    QWidget::keyPressEvent(event);
  else
    QTextEdit::keyPressEvent(event);
}


/*!
 * \author (c) 2002-2007 by the Kopete developers <kopete-devel@kde.org>
 */
static QString makeRegExp( const char *pattern )
{
  const QString urlChar = QLatin1String("\\+\\-\\w\\./#@&;:=\\?~%_,\\!\\$\\*\\(\\)");
  const QString boundaryStart = QString("(^|[^%1])(").arg(urlChar);
  const QString boundaryEnd = QString(")([^%1]|$)").arg(urlChar);

  return boundaryStart + QLatin1String(pattern) + boundaryEnd;
}


/*!
 * \brief Преобразование простых ссылок в html ссылки.
 * \author (c) 2002-2007 by the Kopete developers <kopete-devel@kde.org>
 */
QString InputWidget::parseLinks(const QString &message, bool plain)
{
  if (!plain)
  {
    // < in HTML *always* means start-of-tag
    QStringList entries = message.split(QChar('<'), QString::KeepEmptyParts);

    QStringList::Iterator it = entries.begin();

    // first one is different: it doesn't start with an HTML tag.
    if (it != entries.end()) {
      *it = parseLinks( *it, true);
      ++it;
    }

    for ( ; it != entries.end(); ++it ) {
      QString curr = *it;
      // > in HTML means start-of-tag if and only if it's the first one after a <
      int tagclose = curr.indexOf(QChar('>'));
      // no >: the HTML is broken, but we can cope
      if (tagclose == -1)
        continue;
      QString tag = curr.left(tagclose + 1);
      QString body = curr.mid(tagclose + 1);
      *it = tag + parseLinks(body, true);
    }
    return entries.join(QLatin1String("<"));
  }

  QString result = message;

  // common subpatterns - may not contain matching parens!
  const QString name = QLatin1String("[\\w\\+\\-=_\\.]+");
  const QString userAndPassword = QString("(?:%1(?::%1)?\\@)").arg(name);
  const QString urlChar = QLatin1String("\\+\\-\\w\\./#@&;:=\\?~%_,\\!\\$\\*\\(\\)");
  const QString urlSection = QString("[%1]+").arg(urlChar);
  const QString domain = QLatin1String("[\\-\\w_]+(?:\\.[\\-\\w_]+)+");

  //Replace http/https/ftp links:
  // Replace (stuff)://[user:password@](linkstuff) with a link
  result.replace(
    QRegExp(makeRegExp("\\w+://%1?\\w%2").arg(userAndPassword, urlSection)),
    QLatin1String("\\1<a href=\"\\2\" title=\"\\2\">\\2</a>\\3"));

  // Replace www.X.Y(linkstuff) with a http: link
  result.replace(
    QRegExp(makeRegExp("%1?www\\.%2%3").arg(userAndPassword, domain, urlSection)),
    QLatin1String("\\1<a href=\"http://\\2\" title=\"http://\\2\">\\2</a>\\3"));

  //Replace Email Links
  // Replace user@domain with a mailto: link
  result.replace(
    QRegExp(makeRegExp("%1@%2").arg(name, domain)),
    QLatin1String("\\1<a href=\"mailto:\\2\" title=\"mailto:\\2\">\\2</a>\\3"));

  //Workaround for Bug 85061: Highlighted URLs adds a ' ' after the URL itself
  // the trailing  &nbsp; is included in the url.
  result.replace(QRegExp(QLatin1String("(<a href=\"[^\"]+)(&nbsp;)(\")")), QLatin1String("\\1\\3"));

  return result;
}


/*!
 * \brief Вставка следующего отправленного сообщения.
 */
void InputWidget::nextMsg()
{
  if (m_current + 1 < m_msg.count()) {
    ++m_current;
    setHtml(m_msg.at(m_current));
    moveCursor(QTextCursor::End);
  }
}


/*!
 * \brief Вставка предыдущего отправленного сообщения.
 */
void InputWidget::prevMsg()
{  
  if (m_current) {
    if (m_current <= m_msg.count()) {
      --m_current;
      setHtml(m_msg.at(m_current));
      moveCursor(QTextCursor::End);
    }
  }
}
