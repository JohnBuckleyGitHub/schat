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

#include <QAction>
#include <QApplication>
#include <QFile>
#include <QKeyEvent>
#include <QMenu>

#include "ChatCore.h"
#include "net/packets/message.h"
#include "text/HtmlFilter.h"
#include "ui/InputWidget.h"

InputWidget::InputWidget(QWidget *parent)
  : QTextEdit(parent)
  , m_current(0)
{
  #if defined(Q_OS_MAC)
  setAttribute(Qt::WA_MacShowFocusRect, true);
  setFrameShape(QFrame::NoFrame);
  setFrameShadow(QFrame::Plain);
  #endif
  m_default = currentCharFormat();

  setHeight(1);

  QFile file(QLatin1String(":/css/input.css"));
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    document()->setDefaultStyleSheet(file.readAll());
  }

  document()->setDocumentMargin(2);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  connect(this, SIGNAL(textChanged()), SLOT(textChanged()));

  createActions();
}


void InputWidget::setMsg(int index)
{
  if (index < m_history.size()) {
    m_current = index;
    setHtml(m_history.at(m_current));
    moveCursor(QTextCursor::End);
  }
}


void InputWidget::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QTextEdit::changeEvent(event);
}


void InputWidget::contextMenuEvent(QContextMenuEvent *event)
{
  bool selection = textCursor().hasSelection();
  QMenu menu(this);
  connect(&menu, SIGNAL(triggered(QAction *)), SLOT(menuTriggered(QAction *)));

  if (selection) {
    menu.addAction(m_cut);
    menu.addAction(m_copy);
  }

  if (canPaste())
    menu.addAction(m_paste);

  if (document()->toPlainText().size()) {
    menu.addSeparator();
    menu.addAction(m_clear);
    menu.addAction(m_selectAll);
  }

  if (menu.actions().size())
    menu.exec(event->globalPos());
}


void InputWidget::keyPressEvent(QKeyEvent *event)
{
  int key = event->key();
  Qt::KeyboardModifiers modifiers = event->modifiers();

  if (key == Qt::Key_Return && modifiers == Qt::NoModifier) {
    send();
  }
  else if (key == Qt::Key_Return && (modifiers == Qt::ControlModifier || modifiers == Qt::ShiftModifier)) {
    QKeyEvent *e = new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QTextEdit::keyPressEvent(e);
    delete e;
  }
  else if (key == Qt::Key_Up && modifiers == Qt::ControlModifier) {
    nextMsg();
  }
  else if (key == Qt::Key_Down && modifiers == Qt::ControlModifier) {
    prevMsg();
  }
  else if (key == Qt::Key_Tab) {
    QWidget::keyPressEvent(event);
  }
  else {
    QTextEdit::keyPressEvent(event);
  }
}


/*!
 * Очистка документа.
 */
void InputWidget::clear()
{
  QTextEdit::clear();
  setCurrentCharFormat(m_default);
  emit cursorPositionChanged();
}


void InputWidget::paste()
{
  if (!canPaste())
    return;

  QTextCharFormat format = currentCharFormat();
  QTextEdit::paste();
  setCurrentCharFormat(format);
  emit cursorPositionChanged();
}


void InputWidget::send()
{
  QString html = toHtml();
  HtmlFilter filter(HtmlFilter::ConvertSpacesToNbsp | HtmlFilter::AllowSpanTag);
  QString out = filter.filter(html);
  clear();

  if (out.isEmpty())
    return;

  if (m_history.isEmpty() || m_history.last() != html) {
    if (m_history.size() == 10)
      m_history.takeFirst();

    m_history << html;
    m_current = m_history.count();
  }

  emit send(out);
}


void InputWidget::menuTriggered(QAction *action)
{
  if (action == m_cut) {
    cut();
  }
  else if (action == m_copy) {
    copy();
  }
  else if (action == m_paste) {
    paste();
  }
  else if (action == m_clear) {
    clear();
  }
  else if (action == m_selectAll) {
    selectAll();
  }
}


void InputWidget::textChanged()
{
  int lineCount = document()->lineCount();
  if (m_lines != lineCount)
    setHeight(lineCount);
}


void InputWidget::createActions()
{
  m_cut = new QAction(SCHAT_ICON(EditCut), tr("Cut"), this);
  m_copy = new QAction(SCHAT_ICON(EditCopy), tr("Copy"), this);
  m_paste = new QAction(SCHAT_ICON(EditPaste), tr("Paste"), this);
  m_clear = new QAction(SCHAT_ICON(EditClear), tr("Clear"), this);
  m_selectAll = new QAction(SCHAT_ICON(EditSelectAll), tr("Select All"), this);
}


/*!
 * Вставка следующего отправленного сообщения.
 */
void InputWidget::nextMsg()
{
  if (m_current + 1 < m_history.count()) {
    ++m_current;
    setMsg(m_current);
  }
}


/*!
 * Вставка предыдущего отправленного сообщения.
 */
void InputWidget::prevMsg()
{
  if (m_current) {
    if (m_current <= m_history.count()) {
      --m_current;
      setMsg(m_current);
    }
  }
}


void InputWidget::retranslateUi()
{
  m_cut->setText(tr("Cut"));
  m_copy->setText(tr("Copy"));
  m_paste->setText(tr("Paste"));
  m_clear->setText(tr("Clear"));
  m_selectAll->setText(tr("Select All"));
}


void InputWidget::setHeight(int lines)
{
  #if defined(Q_OS_MAC)
  static const int correction = 4;
  #else
  static const int correction = 2;
  #endif

  if (lines > 4) {
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    lines = 4;
  }
  else
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  m_lines = lines;
  --lines;
  int fontSize = QFontInfo(currentFont()).pixelSize();
  int height = (fontSize * 2 - correction) + fontSize * lines;

  setMinimumHeight(height);
  setMaximumHeight(height);
}
