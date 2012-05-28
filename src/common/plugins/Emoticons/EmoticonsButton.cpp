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
#include <QWidgetAction>

#include "EmoticonsButton.h"
#include "sglobal.h"
#include "EmoticonsTabs.h"

EmoticonsButton::EmoticonsButton(QWidget *parent)
  : QToolButton(parent)
{
  m_menu = new QMenu(this);
  m_tabs = new EmoticonsTabs(this);

  QWidgetAction *action = new QWidgetAction(this);
  action->setDefaultWidget(m_tabs);
  m_menu->addAction(action);

  setAutoRaise(true);
  setIcon(QIcon(LS(":/images/emoticons/edit.png")));
  setPopupMode(QToolButton::InstantPopup);
  setMenu(m_menu);

  retranslateUi();
}


void EmoticonsButton::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void EmoticonsButton::retranslateUi()
{
  setToolTip(tr("Emoticons"));
}


EmoticonsAction::EmoticonsAction()
  : ToolBarActionCreator(1100, LS("emoticons"), WidgetType | AutoShow | AutoDelete)
{
}


QWidget* EmoticonsAction::createWidget(QWidget *parent) const
{
  return new EmoticonsButton(parent);
}


QIcon EmoticonsAction::icon() const
{
  return QIcon(LS(":/images/emoticons/edit.png"));
}


QString EmoticonsAction::title() const
{
  return tr("Emoticons");
}
