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

#include <QEvent>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "ChatCore.h"
#include "net/packets/message.h"
#include "ui/ColorButton.h"
#include "ui/InputWidget.h"
#include "ui/SendWidget.h"

SendWidget::SendWidget(QWidget *parent)
  : QWidget(parent)
  , m_color(0)
  , m_strike(0)
{
  m_toolBar = new QToolBar(this);
//  m_toolBar->setAttribute(Qt::WA_NoSystemBackground, false);
//  m_toolBar->installEventFilter(this);
  m_toolBar->setIconSize(QSize(16, 16));

  m_input = new InputWidget(this);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_toolBar);
  mainLay->addWidget(m_input);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  updateStyleSheet();
  fillToolBar();
  retranslateUi();

  connect(m_input, SIGNAL(send(const QString &)), SLOT(send()));
  connect(m_input, SIGNAL(cursorPositionChanged()), SLOT(cursorPositionChanged()));
}


bool SendWidget::event(QEvent *event)
{
  #if defined(Q_WS_WIN)
  if (event->type() == QEvent::ApplicationPaletteChange) {
    updateStyleSheet();
  }
  #endif

  return QWidget::event(event);
}


void SendWidget::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void SendWidget::cursorPositionChanged()
{
  QTextCursor cursor = m_input->textCursor();
  if (cursor.hasSelection()) {
    int position = cursor.position();
    if (position < cursor.anchor())
      cursor.setPosition(position + 1);
  }

  QTextCharFormat charFormat = cursor.charFormat();
  m_bold->setChecked(charFormat.font().bold());
  m_italic->setChecked(charFormat.font().italic());
  m_underline->setChecked(charFormat.font().underline());

  if (m_strike)
    m_strike->setChecked(charFormat.font().strikeOut());

  if (m_color)
    m_color->setAltColor(charFormat.foreground().color());
}


void SendWidget::send()
{
  m_history->clear();

  if (m_sendButton->menu())
    return;

  m_sendButton->setPopupMode(QToolButton::MenuButtonPopup);
  m_sendButton->setMenu(m_history);
  m_toolBar->removeAction(m_sendAction);
  m_toolBar->addAction(m_sendAction);
}


/*!
 * Изменение состояние текса "Полужирный" \a Ctrl+B.
 */
void SendWidget::setBold(bool b)
{
  QTextCharFormat format;
  format.setFontWeight(b ? QFont::Bold : QFont::Normal);

  mergeFormat(format);
}


/*!
 * \brief Изменение состояние текса "Курсив" \a Ctrl+I.
 */
void SendWidget::setItalic(bool b)
{
  QTextCharFormat format;
  format.setFontItalic(b);

  mergeFormat(format);
}


/*!
 * Изменение состояние текса "Зачёркнутый".
 */
void SendWidget::setStrike(bool b)
{
  QTextCharFormat format;
  format.setFontStrikeOut(b);

  mergeFormat(format);
}


void SendWidget::setTextColor(const QColor &color)
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
void SendWidget::setUnderline(bool b)
{
  QTextCharFormat format;
  format.setFontUnderline(b);

  mergeFormat(format);
}


void SendWidget::showHistoryItem()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
    m_input->setMsg(action->data().toInt());
}


/*!
 * Показ меню истории отправленных сообщений.
 *
 * \todo ! Добавить кнопку очистки истории.
 */
void SendWidget::showHistoryMenu()
{
  if (!m_history->isEmpty())
    return;

  QStringList history = m_input->history();
  QFontMetrics fm = fontMetrics();
  QAction *action = 0;

  for (int i = history.size() - 1; i >= 0; --i) {
    action = m_history->addAction(fm.elidedText(MessageUtils::toPlainText(history.at(i)), Qt::ElideMiddle, 150));
    action->setData(i);
    connect(action, SIGNAL(triggered()), SLOT(showHistoryItem()));
  }
}


/*!
 * Заполнение панели инструментов.
 */
void SendWidget::fillToolBar()
{
  m_bold = m_toolBar->addAction(SCHAT_ICON(TextBoldIcon), "", this, SLOT(setBold(bool)));
  m_bold->setCheckable(true);
  m_bold->setShortcut(Qt::CTRL + Qt::Key_B);

  m_italic = m_toolBar->addAction(SCHAT_ICON(TextItalicIcon), "", this, SLOT(setItalic(bool)));
  m_italic->setCheckable(true);
  m_italic->setShortcut(Qt::CTRL + Qt::Key_I);

  m_underline = m_toolBar->addAction(SCHAT_ICON(TextUnderlineIcon), "", this, SLOT(setUnderline(bool)));
  m_underline->setCheckable(true);
  m_underline->setShortcut(Qt::CTRL + Qt::Key_U);

  m_strike = m_toolBar->addAction(SCHAT_ICON(TextStrikeIcon), "", this, SLOT(setStrike(bool)));
  m_strike->setCheckable(true);

  m_toolBar->addSeparator();

  m_color = new ColorButton(m_input->textColor(), this);
  connect(m_color, SIGNAL(newColor(const QColor &)), SLOT(setTextColor(const QColor &)));
  m_toolBar->addWidget(m_color);

  m_history = new QMenu(this);

  m_sendButton = new QToolButton(this);
  m_sendButton->setAutoRaise(true);
  m_sendButton->setIcon(SCHAT_ICON(SendIcon));

  QWidget *stretch = new QWidget(this);
  stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  m_toolBar->addWidget(stretch);
  m_sendAction = m_toolBar->addWidget(m_sendButton);

  connect(m_sendButton, SIGNAL(clicked()), m_input, SLOT(send()));
  connect(m_history, SIGNAL(aboutToShow()), SLOT(showHistoryMenu()));
}


void SendWidget::mergeFormat(const QTextCharFormat &format)
{
  QTextCursor cursor = m_input->textCursor();

  cursor.mergeCharFormat(format);
  m_input->mergeCurrentCharFormat(format);
}


void SendWidget::retranslateUi()
{
  m_bold->setText(tr("Bold"));
  m_italic->setText(tr("Italic"));
  m_underline->setText(tr("Underline"));
  m_sendButton->setToolTip(tr("Send"));

  if (m_strike)
    m_strike->setText(tr("Strikeout"));
}


void SendWidget::updateStyleSheet()
{
  #if !defined(Q_OS_MAC)
  #if defined(Q_WS_WIN)
  m_toolBar->setStyleSheet(QString("QToolBar { background-color: %1; margin:0px; border:0px; }").arg(palette().color(QPalette::Window).name()));
  #else
  m_toolBar->setStyleSheet("QToolBar { margin:0px; border:0px; }");
  #endif
  #endif
}
