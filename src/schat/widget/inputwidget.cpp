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
}


void InputWidget::sendMsg()
{
  processLinks();

  QString html = toHtml();
  html = html.remove("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n");
  html = html.remove(QRegExp("<html><head>*</head>", Qt::CaseInsensitive, QRegExp::Wildcard));
  html = html.remove(QRegExp("<body[^>]*>"));
  html = html.remove(QRegExp("<p[^>]*>"));
  html = html.remove(QChar('\n'));
  html = html.remove("</p></body></html>");

  if (html.isEmpty())
    return;
  
  if (toPlainText().trimmed().isEmpty())
    return;

  html = html.left(8192);

  while (html.contains("<br /><br />"))
    html = html.replace("<br /><br />", "<br />");

  html = html.replace("<br /></span>", "</span>");

  if (html.endsWith("<br />"))
    html = html.left(html.size() - 6);

  m_msg << html;
  m_current = m_msg.count();

  emit sendMsg(html);

  qDebug() << html;
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


/*!
 * \brief Преобразование простых ссылок в html ссылки.
 */
void InputWidget::processLinks()
{
  QTextCursor cursor = document()->rootFrame()->firstCursorPosition();

  forever {
    cursor = document()->find(QRegExp("\\b(http|ftp|https)://[\\S]+"), cursor);

    if(!cursor.isNull())
      cursor.insertHtml((QString("<a href=%1>%2</a>").arg(cursor.selectedText()).arg(cursor.selectedText())));
    else
      break;
  }
}
