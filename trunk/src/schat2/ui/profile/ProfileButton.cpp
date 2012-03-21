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

#include <QMenu>

#include "Profile.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/profile/ProfileButton.h"

ProfileButton::ProfileButton(QWidget *parent)
  : QToolButton(parent)
{
  m_menu = new QMenu(this);

  setText(tr("Add") + LS(" "));
  setIcon(SCHAT_ICON(Add));
  setPopupMode(InstantPopup);
  setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  setMenu(m_menu);

  connect(m_menu, SIGNAL(triggered(QAction *)), SLOT(triggered(QAction *)));
  connect(m_menu, SIGNAL(aboutToShow()), SLOT(rebuild()));
}


void ProfileButton::rebuild()
{
  m_menu->clear();
  QStringList available = Profile::available();
  if (available.isEmpty())
    setVisible(false);

  foreach (QString field, available) {
    QAction *action = m_menu->addAction(Profile::translate(field));
    action->setData(field);
  }
}


void ProfileButton::triggered(QAction *action)
{
  if (action)
    emit add(action->data().toString());
}
