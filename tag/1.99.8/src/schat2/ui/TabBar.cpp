/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#include "ui/TabBar.h"

TabBar::TabBar(QWidget *parent)
  : QTabBar(parent)
{
  setMovable(true);
  setTabsClosable(true);

# if QT_VERSION < 0x040800
  setElideMode(Qt::ElideRight);
# endif
}


void TabBar::tabInserted(int index)
{
  QTabBar::tabInserted(index);

  if (!tabsClosable() && count() > 1)
    setTabsClosable(true);
}


void TabBar::tabRemoved(int index)
{
  QTabBar::tabRemoved(index);

  if (count() < 2)
    setTabsClosable(false);
}
