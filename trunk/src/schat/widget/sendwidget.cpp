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

  QGridLayout *mainLay = new QGridLayout(this);

  #ifndef Q_OS_WINCE
  mainLay->addWidget(m_toolBar, 0, 0);
  mainLay->addWidget(m_input, 1, 0);
  #else
  mainLay->addWidget(m_input, 0, 0);
  mainLay->addWidget(m_toolBar, 1, 0);
  #endif
  if (SimpleSettings->getBool("BigSendButton"))
    mainLay->addWidget(m_sendButton, 0, 1, 2, 1);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

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
  m_toolBar = new QToolBar(this);
  #if !defined(Q_OS_WINCE)
  m_toolBar->setIconSize(QSize(16, 16));
  #elif defined(SCHAT_WINCE_VGA)
  m_toolBar->setIconSize(QSize(36, 36));
  #endif
  m_toolBar->setStyleSheet("QToolBar { margin: 0px; }");

  m_boldAction = m_toolBar->addAction(QIcon(":/images/format-text-bold.png"), tr("Полужирный"), this, SLOT(setBold(bool)));
  m_boldAction->setCheckable(true);
  m_boldAction->setShortcut(Qt::CTRL + Qt::Key_B);

  m_italicAction = m_toolBar->addAction(QIcon(":/images/format-text-italic.png"), tr("Курсив"), this, SLOT(setItalic(bool)));
  m_italicAction->setCheckable(true);
  m_italicAction->setShortcut(Qt::CTRL + Qt::Key_I);

  m_underlineAction = m_toolBar->addAction(QIcon(":/images/format-text-underline.png"), tr("Подчёркнутый"), this, SLOT(setUnderline(bool)));
  m_underlineAction->setCheckable(true);
  m_underlineAction->setShortcut(Qt::CTRL + Qt::Key_U);

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
  m_toolBar->addSeparator();
  m_toolBar->addWidget(m_emoticonButton);

  QWidget *stretch = new QWidget(this);
  QHBoxLayout *stretchLay = new QHBoxLayout(stretch);
  stretchLay->addStretch();
  m_toolBar->addWidget(stretch);

  m_toolBar->addAction(QIcon(":/images/book.png"), tr("Просмотр журнала"), this, SLOT(log()));

  QAction *sendAction = new QAction(QIcon(":/images/go-jump-locationbar-v.png"), tr("Отправить сообщение"), this);
  connect(sendAction, SIGNAL(triggered()), m_input, SLOT(sendMsg()));
  m_sendButton = new QToolButton(this);
  m_sendButton->setDefaultAction(sendAction);
  m_sendButton->setAutoRaise(true);

  if (SimpleSettings->getBool("BigSendButton")) {
    sendAction->setIcon(QIcon(":/images/go-jump-locationbar-v.png"));
    #ifdef SCHAT_WINCE_VGA
    m_sendButton->setIconSize(QSize(55, 72));
    #else
    m_sendButton->setIconSize(QSize(27, 36));
    #endif
  }
  else {
    sendAction->setIcon(QIcon(":/images/go-jump-locationbar.png"));
    m_toolBar->addSeparator();
    m_toolBar->addWidget(m_sendButton);
  }
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
