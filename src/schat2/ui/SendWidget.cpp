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

#include <QEvent>
#include <QMenu>
#include <QToolBar>
#include <QVBoxLayout>

#include "ChatCore.h"
#include "ChatNotify.h"
#include "hooks/SendButton.h"
#include "hooks/ToolBarActions.h"
#include "sglobal.h"
#include "ui/InputWidget.h"
#include "ui/SendWidget.h"

SendWidget *SendWidget::m_self = 0;

SendWidget::SendWidget(QWidget *parent)
  : QWidget(parent)
{
  m_self = this;

  m_layout << LS("bold") << LS("italic") << LS("underline") << LS("strike") << LS("color") << LS("stretch") << LS("send");

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
  add(new SendAction());

  updateStyleSheet();

  foreach (const QString &action, m_layout) {
    add(action);
  }

  connect(m_input, SIGNAL(send(const QString &)), SIGNAL(send(const QString &)));
  connect(ChatNotify::i(), SIGNAL(notify(const Notify &)), SLOT(notify(const Notify &)));
}


void SendWidget::add(const QString &actionName)
{
  if (!m_self->m_names.contains(actionName))
    return;

  m_self->add(m_self->m_names.value(actionName));
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


void SendWidget::notify(const Notify &notify)
{
  if (notify.type() == Notify::InsertText)
    insertHtml(notify.data().toString());
  else if (notify.type() == Notify::SetSendFocus)
    m_input->setFocus();
}


/*!
 * Определяет действие, которое находится, после действия с весом \p weight.
 */
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
  if (action->flags() & ToolBarActionCreator::ActionType) {
    qa = action->createAction(this);
    m_toolBar->insertAction(before(action->weight()), qa);
  }
  else if (action->flags() & ToolBarActionCreator::WidgetType)
    qa = m_toolBar->insertWidget(before(action->weight()), action->createWidget(this));

  if (qa)
    qa->setData(action->name());

  action->setAction(qa);
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
