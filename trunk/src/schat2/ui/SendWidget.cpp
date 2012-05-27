/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include <QDebug>

#include <QEvent>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "ChatCore.h"
#include "ChatNotify.h"
#include "hooks/ToolBarActions.h"
#include "sglobal.h"
#include "text/PlainTextFilter.h"
#include "ui/ChatIcons.h"
#include "ui/InputWidget.h"
#include "ui/SendWidget.h"

SendWidget *SendWidget::m_self = 0;

SendWidget::SendWidget(QWidget *parent)
  : QWidget(parent)
{
  m_self = this;

  m_layout << LS("bold") << LS("italic") << LS("underline") << LS("strike") << LS("color") << LS("stretch");

  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(16, 16));

  m_input = new InputWidget(this);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(m_toolBar);
  mainLay->addWidget(m_input);
  mainLay->setMargin(0);
  mainLay->setSpacing(0);

  add(new TextEditAction(InputWidget::Bold));
  add(new TextEditAction(InputWidget::Italic));
  add(new TextEditAction(InputWidget::Underline));
  add(new TextEditAction(InputWidget::Strike));
  add(new ColorAction());
  add(new StretchAction());

  updateStyleSheet();
  fillToolBar();
  retranslateUi();

  connect(m_input, SIGNAL(send(const QString &)), SLOT(sendMsg(const QString &)));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
}


void SendWidget::add(const QString &actionName)
{
  if (!m_names.contains(actionName))
    return;

  add(m_names.value(actionName));
}


void SendWidget::setInputFocus()
{
  m_input->setFocus();
}


void SendWidget::insertHtml(const QString &text)
{
  m_input->insertHtml(text);
}


bool SendWidget::event(QEvent *event)
{
# if defined(Q_WS_WIN)
  if (event->type() == QEvent::ApplicationPaletteChange) {
    updateStyleSheet();
  }
# endif

  return QWidget::event(event);
}


void SendWidget::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void SendWidget::notify(const Notify &notify)
{
  if (notify.type() == Notify::InsertText)
    insertHtml(notify.data().toString());
  else if (notify.type() == Notify::SetSendFocus)
    m_input->setFocus();
}


void SendWidget::sendMsg(const QString &text)
{
  m_history->clear();
  emit send(text);

  if (m_sendButton->menu())
    return;

  m_sendButton->setPopupMode(QToolButton::MenuButtonPopup);
  m_sendButton->setMenu(m_history);
  m_toolBar->removeAction(m_sendAction);
  m_toolBar->addAction(m_sendAction);
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
    action = m_history->addAction(fm.elidedText(PlainTextFilter::filter(history.at(i)), Qt::ElideMiddle, 150));
    action->setData(i);
    connect(action, SIGNAL(triggered()), SLOT(showHistoryItem()));
  }
}


QAction* SendWidget::before(int weight)
{
  QList<int> keys = m_actions.keys();
  int index = keys.indexOf(weight);
  if (index == -1)
    return 0;

  for (int i = index; i < keys.size(); ++i) {
    QAction *action = m_actions.value(keys.at(i))->action();
    if (action)
      return action;
  }

  return 0;
}


void SendWidget::add(int weight, ToolBarActionCreator *creator)
{
  if (m_actions.contains(weight)) {
    add(++weight, creator);
    return;
  }

  ToolBarAction action = ToolBarAction(creator);
  m_actions[weight] = action;
  m_names[action->name()] = action;
}


/*!
 * Добавления действия на панель инструментов.
 */
void SendWidget::add(ToolBarAction action)
{
  if (!action)
    return;

  if (action->action())
    return;

  QAction *qa = 0;
  if (action->type() == ToolBarActionCreator::Action) {
    qa = action->createAction(this);
    m_toolBar->insertAction(before(action->weight()), qa);
  }
  else
    qa = m_toolBar->insertWidget(before(action->weight()), action->createWidget(this));

  action->setAction(qa);
}


/*!
 * Заполнение панели инструментов.
 */
void SendWidget::fillToolBar()
{
  foreach (const QString &action, m_layout) {
    add(action);
  }

  m_history = new QMenu(this);

  m_sendButton = new QToolButton(this);
  m_sendButton->setAutoRaise(true);
  m_sendButton->setIcon(SCHAT_ICON(Send));
  m_sendAction = m_toolBar->addWidget(m_sendButton);

  connect(m_sendButton, SIGNAL(clicked()), m_input, SLOT(send()));
  connect(m_history, SIGNAL(aboutToShow()), SLOT(showHistoryMenu()));
}


void SendWidget::retranslateUi()
{
  m_sendButton->setToolTip(tr("Send"));
}


void SendWidget::updateStyleSheet()
{
# if !defined(Q_OS_MAC)
#  if defined(Q_WS_WIN)
    m_toolBar->setStyleSheet(QString("QToolBar { background-color: %1; margin:0px; border:0px; }").arg(palette().color(QPalette::Window).name()));
#  else
    m_toolBar->setStyleSheet("QToolBar { margin:0px; border:0px; }");
#  endif
# endif
}
