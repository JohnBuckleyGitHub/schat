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

#include "widget/emoticonselector.h"
#include "widget/sendwidget.h"

/*!
 * \class SendWidget
 * \brief Виджет полностью берущий на себя ввода текста.
 */

/*!
 * \brief Конструктор класса SendWidget.
 */
SendWidget::SendWidget(Settings *settings, QWidget *parent)
  : QWidget(parent), m_settings(settings)
{
  m_input = new InputWidget(this);
  createButtons();

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(m_boldButton);
  buttonLayout->addWidget(m_italicButton);
  buttonLayout->addWidget(m_underlineButton);
  buttonLayout->addWidget(m_emoticonButton);
  buttonLayout->addStretch();
  buttonLayout->addWidget(m_sendButton);
  buttonLayout->setMargin(0);
  buttonLayout->setSpacing(1);
  
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addLayout(buttonLayout);
  mainLayout->addWidget(m_input);  
  mainLayout->setMargin(0);
  mainLayout->setSpacing(1);

  connect(m_input, SIGNAL(sendMsg(const QString &)), SIGNAL(sendMsg(const QString &)));
  connect(m_input, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)), SLOT(currentCharFormatChanged(const QTextCharFormat &)));
}


/*!
 * \brief Обработка изменения формата символов.
 */
void SendWidget::currentCharFormatChanged(const QTextCharFormat &format)
{
  m_boldAction->setChecked(format.font().bold());
  m_italicAction->setChecked(format.font().italic());
  m_underlineAction->setChecked(format.font().underline());
}


/*!
 * \brief Изменение состояние текса "Полужирный" \a Ctrl+B.
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
 * \brief Изменение состояние текса "Подчёркнутый" \a Ctrl+U.
 */
void SendWidget::setUnderline(bool b)
{
  QTextCharFormat format;
  format.setFontUnderline(b);

  mergeFormat(format);
}


/*!
 * \brief Функция создаёт кнопки.
 */
void SendWidget::createButtons()
{
  m_boldAction = new QAction(QIcon(":/images/bold.png"), tr("Полужирный"), this);
  m_boldAction->setCheckable(true);
  m_boldAction->setShortcut(Qt::CTRL + Qt::Key_B);
  m_boldButton = new QToolButton(this);
  m_boldButton->setDefaultAction(m_boldAction);
  m_boldButton->setAutoRaise(true);
  connect(m_boldAction, SIGNAL(triggered(bool)), SLOT(setBold(bool)));

  m_italicAction = new QAction(QIcon(":/images/italic.png"), tr("Курсив"), this);
  m_italicAction->setCheckable(true);
  m_italicAction->setShortcut(Qt::CTRL + Qt::Key_I);
  m_italicButton = new QToolButton(this);
  m_italicButton->setDefaultAction(m_italicAction);
  m_italicButton->setAutoRaise(true);
  connect(m_italicAction, SIGNAL(triggered(bool)), SLOT(setItalic(bool)));

  m_underlineAction = new QAction(QIcon(":/images/underline.png"), tr("Подчёркнутный"), this);
  m_underlineAction->setCheckable(true);
  m_underlineAction->setShortcut(Qt::CTRL + Qt::Key_U);
  m_underlineButton = new QToolButton(this);
  m_underlineButton->setDefaultAction(m_underlineAction);
  m_underlineButton->setAutoRaise(true);
  connect(m_underlineAction, SIGNAL(triggered(bool)), SLOT(setUnderline(bool)));
  
  m_sendAction = new QAction(QIcon(":/images/send.png"), tr("Отправить сообщение"), this);
  m_sendAction->setStatusTip(tr("Отправить сообщение"));
  m_sendButton = new QToolButton(this);
  m_sendButton->setDefaultAction(m_sendAction);
  m_sendButton->setAutoRaise(true);
  connect(m_sendAction, SIGNAL(triggered()), m_input, SLOT(sendMsg()));
  
  m_popup = new QMenu(this);
  m_emoticonSelector = new EmoticonSelector(m_settings, this);
  QWidgetAction *act = new QWidgetAction(this);
  act->setDefaultWidget(m_emoticonSelector);
  m_popup->addAction(act);
  connect(m_popup, SIGNAL(aboutToShow()), m_emoticonSelector, SLOT(prepareList()));
  
  m_emoticonButton = new QToolButton(this);
  m_emoticonButton->setIcon(QIcon(":/images/emoticon.png"));
  m_emoticonButton->setToolTip(tr("Добавить смайлик"));
  m_emoticonButton->setAutoRaise(true);
  m_emoticonButton->setMenu(m_popup);
  m_emoticonButton->setPopupMode(QToolButton::InstantPopup);
}


/*!
 * \brief Слияние формата.
 */
void SendWidget::mergeFormat(const QTextCharFormat &format)
{
  QTextCursor cursor = m_input->textCursor();

  if(!cursor.hasSelection())
    cursor.movePosition(cursor.position() == 0 ? QTextCursor::NextCharacter : QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

  cursor.mergeCharFormat(format);
  m_input->mergeCurrentCharFormat(format);
}
