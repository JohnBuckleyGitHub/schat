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

#include "channellog.h"
#include "widget/inputwidget.h"


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
  document()->setDefaultStyleSheet("a {color:#815d53; text-decoration:none;}");
  #if QT_VERSION >= 0x040500
    document()->setDocumentMargin(2);
  #endif

  createActions();
}


void InputWidget::sendMsg()
{
  QString html = toHtml();
  html = ChannelLog::htmlFilter(html);

  if (html.isEmpty())
    return;

  m_msg << html;
  m_current = m_msg.count();

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
  emit cursorPositionChanged();
}


void InputWidget::paste()
{
  if (canPaste()) {
    QTextCharFormat format = currentCharFormat();
    QTextEdit::paste();
    setCurrentCharFormat(format);
    emit cursorPositionChanged();
  }
}


/*!
 * Контекстное меню
 */
void InputWidget::contextMenuEvent(QContextMenuEvent *event)
{
  bool selection = textCursor().hasSelection();

  m_cutAction->setEnabled(selection);
  m_copyAction->setEnabled(selection);
  m_pasteAction->setEnabled(canPaste());

  bool empty = (bool) document()->toPlainText().size();
  m_clearAction->setEnabled(empty);
  m_selectAllAction->setEnabled(empty);

  QMenu menu(this);
  menu.addAction(m_cutAction);
  menu.addAction(m_copyAction);
  menu.addAction(m_pasteAction);
  menu.addSeparator();
  menu.addAction(m_clearAction);
  menu.addAction(m_selectAllAction);
  menu.exec(event->globalPos());
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
  else if (key == "Ctrl+C")
    emit needCopy();
  else if (key == "Ctrl+V")
    paste();
  else if (event->key() == Qt::Key_Tab)
    QWidget::keyPressEvent(event);
  else
    QTextEdit::keyPressEvent(event);
}


/*!
 * Создание объектов \a QAction.
 */
void InputWidget::createActions()
{
  m_cutAction = new QAction(QIcon(":/images/editcut.png"), tr("&Вырезать"), this);
  m_cutAction->setShortcut(Qt::CTRL + Qt::Key_X);
  connect(m_cutAction, SIGNAL(triggered()), SLOT(cut()));

  m_copyAction = new QAction(QIcon(":/images/editcopy.png"), tr("&Копировать"), this);
  m_copyAction->setShortcut(Qt::CTRL + Qt::Key_C);
  connect(m_copyAction, SIGNAL(triggered()), SIGNAL(needCopy()));

  m_pasteAction = new QAction(QIcon(":/images/editpaste.png"), tr("&Вставить"), this);
  m_pasteAction->setShortcut(Qt::CTRL + Qt::Key_V);
  connect(m_pasteAction, SIGNAL(triggered()), SLOT(paste()));

  m_clearAction = new QAction(QIcon(":/images/editclear.png"), tr("&Очистить"), this);
  connect(m_clearAction, SIGNAL(triggered()), SLOT(clearMsg()));

  m_selectAllAction = new QAction(tr("&Выделить всё"), this);
  m_selectAllAction->setShortcut(Qt::CTRL + Qt::Key_A);
  connect(m_selectAllAction, SIGNAL(triggered()), SLOT(selectAll()));
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
