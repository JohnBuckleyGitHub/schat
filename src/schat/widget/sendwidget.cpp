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
  : QWidget(parent),
  m_input(new InputWidget(this))
{
  m_availableActions << "bold" << "italic" << "underline" << "emoticons" << "stretch" << "log" << "send" << "separator";
  initToolBar();
  setSettings();

  QGridLayout *mainLay = new QGridLayout(this);

  #ifndef Q_OS_WINCE
  mainLay->addWidget(m_toolBar, 0, 0);
  mainLay->addWidget(m_input, 1, 0);
  #else
  mainLay->addWidget(m_input, 0, 0);
  mainLay->addWidget(m_toolBar, 1, 0);
  #endif
  if (SimpleSettings->getBool("BigSendButton") && m_send)
    mainLay->addWidget(m_send, 0, 1, 2, 1);
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
  if (m_bold) m_bold->setChecked(charFormat.font().bold());
  if (m_italic) m_italic->setChecked(charFormat.font().italic());
  if (m_underline) m_underline->setChecked(charFormat.font().underline());
}


void SendWidget::log()
{
  QDesktopServices::openUrl(QUrl::fromLocalFile(QApplication::applicationDirPath() + "/log"));
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


void SendWidget::setSettings()
{
  if (m_emoticons) m_emoticons->setEnabled(SimpleSettings->getBool("UseEmoticons"));
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
 * Фильтр событий.
 */
bool SendWidget::eventFilter(QObject *object, QEvent *event)
{
//  if (event->type() == QEvent::ContextMenu) {
//    qDebug() << "eventFilter()" << object;
//    QMenu menu(this);
//    menu.addAction("тест");
//
//    QContextMenuEvent *menuEvent = static_cast<QContextMenuEvent *>(event);
//    menu.exec(menuEvent->globalPos());
//    return true;
//  }

  return QWidget::eventFilter(object, event);
}


/*!
 * Создание кнопки на панели инструментов.
 *
 * \param name Символьное имя кнопки.
 * \return Указатель на созданный объект QAction, или 0 в случае ошибки.
 */
QAction* SendWidget::createAction(const QString &name)
{
  QString lowerName = name.toLower();
  if (!m_availableActions.contains(name))
    return 0;

  QAction *action = 0;

  if (lowerName == "bold") {
    action = m_toolBar->addAction(QIcon(":/images/format-text-bold.png"), tr("Полужирный"), this, SLOT(setBold(bool)));
    action->setCheckable(true);
    action->setShortcut(Qt::CTRL + Qt::Key_B);
    m_bold = action;
  }
  else if (lowerName == "italic") {
    action = m_toolBar->addAction(QIcon(":/images/format-text-italic.png"), tr("Курсив"), this, SLOT(setItalic(bool)));
    action->setCheckable(true);
    action->setShortcut(Qt::CTRL + Qt::Key_I);
    m_italic = action;
  }
  else if (lowerName == "underline") {
    action = m_toolBar->addAction(QIcon(":/images/format-text-underline.png"), tr("Подчёркнутый"), this, SLOT(setUnderline(bool)));
    action->setCheckable(true);
    action->setShortcut(Qt::CTRL + Qt::Key_U);
    m_underline = action;
  }
  else if (lowerName == "separator") {
    action = m_toolBar->addSeparator();
  }
  else if (lowerName == "emoticons") {
    QMenu *menu = new QMenu(this);
    EmoticonSelector *emoticonSelector = new EmoticonSelector(this);
    QWidgetAction *act = new QWidgetAction(this);
    act->setDefaultWidget(emoticonSelector);
    menu->addAction(act);
    connect(menu, SIGNAL(aboutToShow()), emoticonSelector, SLOT(prepareList()));
    connect(menu, SIGNAL(aboutToHide()), emoticonSelector, SLOT(aboutToHide()));
    connect(emoticonSelector, SIGNAL(itemSelected(const QString &)), m_input, SLOT(insertPlainText(const QString &)));

    m_emoticons = new QToolButton(this);
    m_emoticons->setIcon(QIcon(":/images/emoticon.png"));
    m_emoticons->setToolTip(tr("Добавить смайлик"));
    m_emoticons->setAutoRaise(true);
    m_emoticons->setMenu(menu);
    m_emoticons->setPopupMode(QToolButton::InstantPopup);
    m_emoticons->setShortcut(Qt::CTRL + Qt::Key_E);
    action = m_toolBar->addWidget(m_emoticons);
  }
  else if (lowerName == "stretch") {
    QWidget *stretch = new QWidget(this);
    QHBoxLayout *stretchLay = new QHBoxLayout(stretch);
    stretchLay->addStretch();
    action = m_toolBar->addWidget(stretch);
  }
  else if (lowerName == "log") {
    action = m_toolBar->addAction(QIcon(":/images/book.png"), tr("Просмотр журнала"), this, SLOT(log()));
  }
  else if (lowerName == "send") {
    m_send = new QToolButton(this);
    m_send->setToolTip(tr("Отправить сообщение"));
    m_send->setAutoRaise(true);
    connect(m_send, SIGNAL(clicked()), m_input, SLOT(sendMsg()));

    if (SimpleSettings->getBool("BigSendButton")) {
      m_send->setIcon(QIcon(":/images/go-jump-locationbar-v.png"));
      #ifdef SCHAT_WINCE_VGA
      m_send->setIconSize(QSize(55, 72));
      #else
      m_send->setIconSize(QSize(27, 36));
      #endif
    }
    else {
      m_send->setIcon(QIcon(":/images/go-jump-locationbar.png"));
      action = m_toolBar->addWidget(m_send);
    }
  }

  if (action) {
    m_availableActions.removeAll(lowerName);
    action->setData(lowerName);
    if (lowerName != "separator" && !m_availableActions.contains("separator"))
      m_availableActions << "separator";
  }

  return action;
}


/*!
 * Создание кнопок на панели инструментов по списку.
 */
void SendWidget::buildToolBar(const QStringList &actions)
{
  foreach (QString name, actions) {
    createAction(name);
  }
}


/*!
 * Инициализация панели инструментов.
 */
void SendWidget::initToolBar()
{
  m_toolBar = new QToolBar(this);
  m_toolBar->installEventFilter(this);
  #if !defined(Q_OS_WINCE)
  m_toolBar->setIconSize(QSize(16, 16));
  #elif defined(SCHAT_WINCE_VGA)
  m_toolBar->setIconSize(QSize(36, 36));
  #endif
  m_toolBar->setStyleSheet("QToolBar { margin: 0px; }");

  buildToolBar(SimpleSettings->getList("ToolBarLayout"));
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
