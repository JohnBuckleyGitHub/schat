/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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


#include <QContextMenuEvent>
#include <QMenu>
//#include <QDebug>

#include "channellog.h"
#include "simplechatapp.h"
#include "widget/inputwidget.h"

/*!
 * \brief Конструктор класса InputWidget.
 */
InputWidget::InputWidget(QWidget *parent)
  : QTextEdit(parent)
{
  #if defined(Q_OS_MAC)
  setAttribute(Qt::WA_MacShowFocusRect, true);
  setFrameShape(QFrame::NoFrame);
  setFrameShadow(QFrame::Plain);
  #endif
  detectMinimumHeight();
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


void InputWidget::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QTextEdit::changeEvent(event);
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
  int key = event->key();
  Qt::KeyboardModifiers modifiers = event->modifiers();

  if (key == Qt::Key_Return && modifiers == Qt::NoModifier)
    sendMsg();
  #if QT_VERSION >= 0x040500
  else if (key == Qt::Key_Return && modifiers == Qt::ControlModifier)
    QApplication::postEvent(this, new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::ShiftModifier));
  #endif
  else if (key == Qt::Key_Up && modifiers == Qt::ControlModifier)
    nextMsg();
  else if (key == Qt::Key_Down && modifiers == Qt::ControlModifier)
    prevMsg();
  else if (event->matches(QKeySequence::Copy))
    emit needCopy();
  else if (event->matches(QKeySequence::Paste))
    paste();
  else if (key == Qt::Key_1 && modifiers == Qt::ControlModifier)
    emit statusShortcut(1);
  else if (key == Qt::Key_2 && modifiers == Qt::ControlModifier)
    emit statusShortcut(2);
  else if (key == Qt::Key_3 && modifiers == Qt::ControlModifier)
    emit statusShortcut(3);
  else if (key == Qt::Key_0 && modifiers == Qt::ControlModifier)
    emit statusShortcut(0);
  else if (key == Qt::Key_Tab)
    QWidget::keyPressEvent(event);
  else
    QTextEdit::keyPressEvent(event);
}


/*!
 * Создание объектов \a QAction.
 */
void InputWidget::createActions()
{
  m_cutAction = new QAction(SimpleChatApp::iconFromTheme("edit-cut"), "", this);
  m_cutAction->setShortcut(QKeySequence::Cut);
  connect(m_cutAction, SIGNAL(triggered()), SLOT(cut()));

  m_copyAction = new QAction(SimpleChatApp::iconFromTheme("edit-copy"), "", this);
  m_copyAction->setShortcut(QKeySequence::Copy);
  connect(m_copyAction, SIGNAL(triggered()), SIGNAL(needCopy()));

  m_pasteAction = new QAction(SimpleChatApp::iconFromTheme("edit-paste"), "", this);
  m_pasteAction->setShortcut(QKeySequence::Paste);
  connect(m_pasteAction, SIGNAL(triggered()), SLOT(paste()));

  m_clearAction = new QAction(SimpleChatApp::iconFromTheme("edit-clear"), "", this);
  connect(m_clearAction, SIGNAL(triggered()), SLOT(clearMsg()));

  m_selectAllAction = new QAction(SimpleChatApp::iconFromTheme("edit-select-all"), "", this);
  m_selectAllAction->setShortcut(QKeySequence::SelectAll);
  connect(m_selectAllAction, SIGNAL(triggered()), SLOT(selectAll()));
}


void InputWidget::detectMinimumHeight()
{
  #if defined(Q_OS_WINCE)
   #if defined(SCHAT_WINCE_VGA)
    static const int correction = 10;
   #else
    static const int correction = 4;
   #endif
  #else
    #if defined(Q_OS_MAC)
    static const int correction = 4;
    #else
    static const int correction = 0;
    #endif
  #endif
  QFontInfo fontInfo(currentFont());
  setMinimumHeight(fontInfo.pixelSize() * 2 - correction);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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


void InputWidget::retranslateUi()
{
  m_cutAction->setText(tr("Cu&t"));
  m_copyAction->setText(tr("&Copy"));
  m_pasteAction->setText(tr("&Paste"));
  m_clearAction->setText(tr("Clear"));
  m_selectAllAction->setText(tr("Select All"));
}
