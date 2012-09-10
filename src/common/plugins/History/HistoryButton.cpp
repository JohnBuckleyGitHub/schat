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

#include <QMenu>

#include "HistoryButton.h"
#include "net/SimpleID.h"
#include "sglobal.h"

HistoryButton::HistoryButton(QWidget *parent)
  : QToolButton(parent)
{
  m_menu = new QMenu(this);

  setAutoRaise(true);
  setIcon(QIcon(LS(":/images/History/book.png")));
  setPopupMode(QToolButton::InstantPopup);
  setMenu(m_menu);
}


HistoryAction::HistoryAction()
  : ToolBarActionCreator(10500, LS("history"), WidgetType | AutoShow | AutoDelete)
{
}


QWidget* HistoryAction::createWidget(QWidget *parent) const
{
  return new HistoryButton(parent);
}


bool HistoryAction::isVisible(const QString &type, const QByteArray &id)
{
  Q_UNUSED(type)

  if (SimpleID::typeOf(id) == SimpleID::UserId || SimpleID::typeOf(id) == SimpleID::ChannelId)
    return true;

  return false;
}


QIcon HistoryAction::icon() const
{
  return QIcon(LS(":/images/History/book.png"));
}


QString HistoryAction::title() const
{
  return tr("History");
}
