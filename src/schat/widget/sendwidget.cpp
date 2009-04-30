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

#include "emoticons/emoticonselector.h"
#include "settings.h"
#include "widget/sendwidget.h"

/*!
 * \brief Конструктор класса SendWidget.
 */
SendWidget::SendWidget(QWidget *parent)
  : QWidget(parent)
{
  m_input = new InputWidget(this);
  createButtons();
  setSettings();

  QHBoxLayout *buttonLay = new QHBoxLayout;
  buttonLay->addWidget(m_format);
  buttonLay->addStretch();
  buttonLay->addWidget(m_tools);
  buttonLay->setMargin(0);
  buttonLay->setSpacing(1);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addLayout(buttonLay);
  mainLay->addWidget(m_input);
  mainLay->setMargin(0);
  mainLay->setSpacing(1);

  connect(m_input, SIGNAL(sendMsg(const QString &)), SIGNAL(sendMsg(const QString &)));
  connect(m_input, SIGNAL(needCopy()), SIGNAL(needCopy()));
  connect(m_input, SIGNAL(statusShortcut(int)), SIGNAL(statusShortcut(int)));
  connect(SimpleSettings, SIGNAL(changed(int)), SLOT(setSettings()));
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
  QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/log"));
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
  m_emoticonButton->setEnabled(SimpleSettings->getBool("UseEmoticons"));
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
  m_format = new QToolBar(this);
  m_format->setIconSize(QSize(16, 16));
  m_format->setStyleSheet("QToolBar { margin: 0px; }");

  m_boldAction = m_format->addAction(QIcon(":/images/format-text-bold.png"), tr("Полужирный"), this, SLOT(setBold(bool)));
  m_boldAction->setCheckable(true);
  m_boldAction->setShortcut(Qt::CTRL + Qt::Key_B);

  m_italicAction = m_format->addAction(QIcon(":/images/format-text-italic.png"), tr("Курсив"), this, SLOT(setItalic(bool)));
  m_italicAction->setCheckable(true);
  m_italicAction->setShortcut(Qt::CTRL + Qt::Key_I);

  m_underlineAction = m_format->addAction(QIcon(":/images/format-text-underline.png"), tr("Подчёркнутый"), this, SLOT(setUnderline(bool)));
  m_underlineAction->setCheckable(true);
  m_underlineAction->setShortcut(Qt::CTRL + Qt::Key_U);

  m_tools = new QToolBar(this);
  m_tools->setIconSize(QSize(16, 16));
  m_tools->setStyleSheet("QToolBar { margin: 0px; }");

  m_tools->addAction(QIcon(":/images/book.png"), tr("Просмотр журнала"), this, SLOT(log()));
  m_tools->addSeparator();
  m_tools->addAction(QIcon(":/images/go-jump-locationbar.png"), tr("Отправить сообщение"), m_input, SLOT(sendMsg()));

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
  m_format->addSeparator();
  m_format->addWidget(m_emoticonButton);
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
