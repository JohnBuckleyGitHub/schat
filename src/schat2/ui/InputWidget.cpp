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
#include <QScrollBar>
#include <QToolBar>
#include <QWidgetAction>

#include "ChatCore.h"
#include "net/packets/message.h"
#include "text/HtmlFilter.h"
#include "ui/ColorButton.h"
#include "ui/InputWidget.h"

InputWidget::InputWidget(QWidget *parent)
  : QTextEdit(parent)
  , m_emptySend(false)
  , m_scalable(true)
  , m_current(0)
  , m_lines(0)
  , m_maxLines(5)
  , m_menu(0)
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
  connect(this, SIGNAL(cursorPositionChanged()), SLOT(cursorPositionChanged()));

  createActions();
}


ColorButton *InputWidget::color()
{
  m_color->setVisible(true);
  return m_color;
}


void InputWidget::adjustHeight()
{
  m_scalable = false;
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  while (verticalScrollBar()->isVisible() && m_lines < m_maxLines) {
    setHeight(m_lines + 1);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  }
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


/*!
 * Показ контекстного меню.
 * \todo Добавить возможность устанавливать произвольную высоту текста по умолчанию.
 */
void InputWidget::contextMenuEvent(QContextMenuEvent *event)
{
  bool selection = textCursor().hasSelection();
  m_menu = new QMenu(this);
  connect(m_menu, SIGNAL(triggered(QAction *)), SLOT(menuTriggered(QAction *)));

  m_menu->addAction(m_action);
  m_menu->addSeparator();

  if (selection) {
    m_menu->addAction(m_cut);
    m_menu->addAction(m_copy);
  }

  if (canPaste())
    m_menu->addAction(m_paste);

  if (document()->toPlainText().size()) {
    m_menu->addSeparator();
    m_menu->addAction(m_clear);
    m_menu->addAction(m_selectAll);
  }

  if (m_menu->actions().size())
    m_menu->exec(event->globalPos());

  m_menu->deleteLater();
  m_menu = 0;
}


void InputWidget::focusOutEvent(QFocusEvent *event)
{
  QTextEdit::focusOutEvent(event);

  if (!m_menu)
    emit focusOut();
}


void InputWidget::keyPressEvent(QKeyEvent *event)
{
  int key = event->key();
  Qt::KeyboardModifiers modifiers = event->modifiers();

  if (key == Qt::Key_Return && modifiers == Qt::NoModifier) {
    send();
    return;
  }

  if (key == Qt::Key_Return && (modifiers == Qt::ControlModifier || modifiers == Qt::ShiftModifier)) {
    QKeyEvent *e = new QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QTextEdit::keyPressEvent(e);
    delete e;
    return;
  }

  if (event->matches(QKeySequence::Copy)) {
    if (textCursor().hasSelection())
      QTextEdit::keyPressEvent(event);
    else
      ChatCore::i()->startNotify(ChatCore::CopyRequestNotice);

    return;
  }

  if (event->matches(QKeySequence::Paste)) {
    paste();
    return;
  }

  if (modifiers == Qt::ControlModifier) {
    if (key == Qt::Key_Up) {
      nextMsg();
    }
    else if (key == Qt::Key_Down) {
      prevMsg();
    }
    else if (key == Qt::Key_B) {
      m_format.at(Bold)->toggle();
      setBold(m_format.at(Bold)->isChecked());
    }
    else if (key == Qt::Key_I) {
      m_format.at(Italic)->toggle();
      setItalic(m_format.at(Italic)->isChecked());
    }
    else if (key == Qt::Key_U) {
      m_format.at(Underline)->toggle();
      setUnderline(m_format.at(Underline)->isChecked());
    }
    else
      return QTextEdit::keyPressEvent(event);
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
  setHeight(1);

  if (out.isEmpty()) {
    if (m_emptySend)
      emit send(out);

    return;
  }

  if (m_history.isEmpty() || m_history.last() != html) {
    if (m_history.size() == 10)
      m_history.takeFirst();

    m_history << html;
    m_current = m_history.count();
  }

  emit send(out);
}


void InputWidget::cursorPositionChanged()
{
  QTextCursor cursor = textCursor();
  if (cursor.hasSelection()) {
    int position = cursor.position();
    if (position < cursor.anchor())
      cursor.setPosition(position + 1);
  }

  QTextCharFormat charFormat = cursor.charFormat();
  m_format.at(Bold)->setChecked(charFormat.font().bold());
  m_format.at(Italic)->setChecked(charFormat.font().italic());
  m_format.at(Underline)->setChecked(charFormat.font().underline());
  m_format.at(Strike)->setChecked(charFormat.font().strikeOut());
  m_color->setAltColor(charFormat.foreground().color());
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


/*!
 * Изменение состояние текса "Полужирный" \a Ctrl+B.
 */
void InputWidget::setBold(bool bold)
{
  QTextCharFormat format;
  format.setFontWeight(bold ? QFont::Bold : QFont::Normal);

  mergeFormat(format);
}


/*!
 * Изменение состояние текса "Курсив" \a Ctrl+I.
 */
void InputWidget::setItalic(bool italic)
{
  QTextCharFormat format;
  format.setFontItalic(italic);

  mergeFormat(format);
}


/*!
 * Изменение состояние текса "Зачёркнутый".
 */
void InputWidget::setStrike(bool strike)
{
  QTextCharFormat format;
  format.setFontStrikeOut(strike);

  mergeFormat(format);
}


void InputWidget::setTextColor(const QColor &color)
{
  if (!color.isValid())
    return;

  QTextCharFormat format;
  format.setForeground(color);

  mergeFormat(format);
}


/*!
 * Изменение состояние текса "Подчёркнутый" \a Ctrl+U.
 */
void InputWidget::setUnderline(bool underline)
{
  QTextCharFormat format;
  format.setFontUnderline(underline);

  mergeFormat(format);
}


void InputWidget::textChanged()
{
  if (!m_scalable)
    return;

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

  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(16, 16));
  m_toolBar->setStyleSheet("QToolBar { margin:0px; border:0px; }");

  m_action = new QWidgetAction(this);
  m_action->setDefaultWidget(m_toolBar);

  QAction *action;
  action = new QAction(SCHAT_ICON(TextBoldIcon), tr("Bold"), this);
  action->setCheckable(true);
  connect(action, SIGNAL(triggered(bool)), SLOT(setBold(bool)));
  m_toolBar->addAction(action);
  m_format.append(action);

  action = new QAction(SCHAT_ICON(TextItalicIcon), tr("Italic"), this);
  action->setCheckable(true);
  connect(action, SIGNAL(triggered(bool)), SLOT(setItalic(bool)));
  m_toolBar->addAction(action);
  m_format.append(action);

  action = new QAction(SCHAT_ICON(TextUnderlineIcon), tr("Underline"), this);
  action->setCheckable(true);
  connect(action, SIGNAL(triggered(bool)), SLOT(setUnderline(bool)));
  m_toolBar->addAction(action);
  m_format.append(action);

  action = m_toolBar->addAction(SCHAT_ICON(TextStrikeIcon), tr("Strikeout"), this, SLOT(setStrike(bool)));
  action->setCheckable(true);
  connect(action, SIGNAL(triggered(bool)), SLOT(setStrike(bool)));
  m_toolBar->addAction(action);
  m_format.append(action);

  m_color = new ColorButton(textColor(), this);
  m_color->setVisible(false);
  connect(m_color, SIGNAL(newColor(const QColor &)), SLOT(setTextColor(const QColor &)));
}


void InputWidget::mergeFormat(const QTextCharFormat &format)
{
  QTextCursor cursor = textCursor();

  cursor.mergeCharFormat(format);
  mergeCurrentCharFormat(format);
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
  m_format.at(Bold)->setText(tr("Bold"));
  m_format.at(Italic)->setText(tr("Italic"));
  m_format.at(Underline)->setText(tr("Underline"));
  m_format.at(Strike)->setText(tr("Strikeout"));
}


void InputWidget::setHeight(int lines)
{
  #if defined(Q_OS_MAC)
  static const int correction = 4;
  #else
  static const int correction = 2;
  #endif

  if (lines > m_maxLines) {
    lines = m_maxLines;
    if (lines > 2)
      setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  }
  else
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  if (m_lines == lines)
    return;

  m_lines = lines;
  --lines;
  int fontSize = QFontInfo(currentFont()).pixelSize();
  int height = (fontSize * 2 - correction) + fontSize * lines;
  if (m_lines > 1) {
    height += 2 * m_lines;
  }

  setMinimumHeight(height);
  setMaximumHeight(height);
}
