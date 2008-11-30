/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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
#include "settings.h"

/*!
 * \brief Конструктор класса SendWidget.
 */
SendWidget::SendWidget(QWidget *parent)
  : QWidget(parent)
{
  m_settings = settings;
  m_input = new InputWidget(this);
  createButtons();
  setSettings();

  QHBoxLayout *buttonLay = new QHBoxLayout;
  buttonLay->addWidget(m_boldButton);
  buttonLay->addWidget(m_italicButton);
  buttonLay->addWidget(m_underlineButton);
  buttonLay->addWidget(m_emoticonButton);
  buttonLay->addStretch();
  buttonLay->addWidget(m_logButton);
  buttonLay->addWidget(m_sendButton);
  buttonLay->setMargin(0);
  buttonLay->setSpacing(1);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addLayout(buttonLay);
  mainLay->addWidget(m_input);
  mainLay->setMargin(0);
  mainLay->setSpacing(1);

  connect(m_input, SIGNAL(sendMsg(const QString &)), SIGNAL(sendMsg(const QString &)));
  connect(m_input, SIGNAL(needCopy()), SIGNAL(needCopy()));
  connect(m_settings, SIGNAL(changed(int)), SLOT(setSettings()));
  connect(m_input, SIGNAL(cursorPositionChanged()), SLOT(cursorPositionChanged()));
}


/*!
 * \brief Обработка изменения позиции курсора для обновления кнопок.
 */
void SendWidget::cursorPositionChanged()
{
  QTextCursor cursor = m_input->textCursor();
  if (cursor.hasSelection()) {
    int position = cursor.position();
    if (position < cursor.anchor())
      cursor.setPosition(position + 1);
  }

  QTextCharFormat charFormat = cursor.charFormat();
  m_boldAction->setChecked(charFormat.font().bold());
  m_italicAction->setChecked(charFormat.font().italic());
  m_underlineAction->setChecked(charFormat.font().underline());
}


void SendWidget::log()
{
  QDesktopServices::openUrl(QUrl::fromLocalFile(qApp->applicationDirPath() + "/log"));
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


void SendWidget::setSettings()
{
  m_emoticonButton->setEnabled(m_settings->getBool("UseEmoticons"));
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
  m_boldAction = new QAction(QIcon(":/images/format-text-bold.png"), tr("Полужирный"), this);
  m_boldAction->setCheckable(true);
  m_boldAction->setShortcut(Qt::CTRL + Qt::Key_B);
  m_boldButton = new QToolButton(this);
  m_boldButton->setDefaultAction(m_boldAction);
  m_boldButton->setAutoRaise(true);
  connect(m_boldAction, SIGNAL(triggered(bool)), SLOT(setBold(bool)));

  m_italicAction = new QAction(QIcon(":/images/format-text-italic.png"), tr("Курсив"), this);
  m_italicAction->setCheckable(true);
  m_italicAction->setShortcut(Qt::CTRL + Qt::Key_I);
  m_italicButton = new QToolButton(this);
  m_italicButton->setDefaultAction(m_italicAction);
  m_italicButton->setAutoRaise(true);
  connect(m_italicAction, SIGNAL(triggered(bool)), SLOT(setItalic(bool)));

  m_underlineAction = new QAction(QIcon(":/images/format-text-underline.png"), tr("Подчёркнутый"), this);
  m_underlineAction->setCheckable(true);
  m_underlineAction->setShortcut(Qt::CTRL + Qt::Key_U);
  m_underlineButton = new QToolButton(this);
  m_underlineButton->setDefaultAction(m_underlineAction);
  m_underlineButton->setAutoRaise(true);
  connect(m_underlineAction, SIGNAL(triggered(bool)), SLOT(setUnderline(bool)));

  m_logAction = new QAction(QIcon(":/images/book.png"), tr("Просмотр журнала"), this);
  m_logButton = new QToolButton(this);
  m_logButton->setDefaultAction(m_logAction);
  m_logButton->setAutoRaise(true);
  connect(m_logAction, SIGNAL(triggered()), SLOT(log()));

  m_sendAction = new QAction(QIcon(":/images/go-jump-locationbar.png"), tr("Отправить сообщение"), this);
  m_sendButton = new QToolButton(this);
  m_sendButton->setDefaultAction(m_sendAction);
  m_sendButton->setAutoRaise(true);
  connect(m_sendAction, SIGNAL(triggered()), m_input, SLOT(sendMsg()));

  m_popup = new QMenu(this);
  m_emoticonSelector = new EmoticonSelector(this);
  QWidgetAction *act = new QWidgetAction(this);
  act->setDefaultWidget(m_emoticonSelector);
  m_popup->addAction(act);
  connect(m_popup, SIGNAL(aboutToShow()), m_emoticonSelector, SLOT(prepareList()));
  connect(m_popup, SIGNAL(aboutToHide()), m_emoticonSelector, SLOT(aboutToHide()));
  connect(m_emoticonSelector, SIGNAL(itemSelected(const QString &)), m_input, SLOT(insertPlainText(const QString &)));

  m_emoticonButton = new QToolButton(this);
  m_emoticonButton->setIcon(QIcon(":/images/emoticon.png"));
  m_emoticonButton->setToolTip(tr("Добавить смайлик"));
  m_emoticonButton->setAutoRaise(true);
  m_emoticonButton->setMenu(m_popup);
  m_emoticonButton->setPopupMode(QToolButton::InstantPopup);
  m_emoticonButton->setShortcut(Qt::CTRL + Qt::Key_E);
}


/*!
 * \brief Слияние формата.
 */
void SendWidget::mergeFormat(const QTextCharFormat &format)
{
  QTextCursor cursor = m_input->textCursor();

  cursor.mergeCharFormat(format);
  m_input->mergeCurrentCharFormat(format);
}
