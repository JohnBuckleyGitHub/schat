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

#include "ShareButton.h"
#include "sglobal.h"

ShareButton::ShareButton(Share *share, QWidget *parent)
  : QToolButton(parent)
  , m_share(share)
{
  setAutoRaise(true);
  setPopupMode(QToolButton::InstantPopup);
}


ShareAction::ShareAction(Share *share)
  : ToolBarActionCreator(1150, LS("share"), WidgetType | AutoShow | AutoDelete)
  , m_icon(LS(":/images/Share/button.png"))
  , m_share(share)
{
}


QWidget* ShareAction::createWidget(QWidget *parent) const
{
  ShareButton *button = new ShareButton(m_share, parent);
  button->setIcon(m_icon);
  return button;
}


QString ShareAction::title() const
{
  return tr("Emoticons");
}
