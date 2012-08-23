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

#include <QProxyStyle>

#include "ui/TabBar.h"

class TabStyle : public QProxyStyle
{
public:
  TabStyle(QStyle *style = 0)
  : QProxyStyle(style)
  {}

  int pixelMetric(PixelMetric metric, const QStyleOption *option = 0, const QWidget *widget = 0) const
  {
    int result = QProxyStyle::pixelMetric(metric, option, widget);
    if (metric == PM_TabBarTabHSpace)
      return result / 2;

#   if defined(Q_WS_WIN)
    else if (metric == PM_TabBarTabVSpace)
      return result + 2;
#   endif

    return result;
  }
};


TabBar::TabBar(QWidget *parent)
  : QTabBar(parent)
{
  setMovable(true);
  setTabsClosable(true);
  setUsesScrollButtons(true);
  setElideMode(Qt::ElideMiddle);

  setStyle(new TabStyle());
}


void TabBar::tabInserted(int index)
{
  QTabBar::tabInserted(index);
  setTabsClosable(count() > 1);
}


void TabBar::tabRemoved(int index)
{
  QTabBar::tabRemoved(index);
  setTabsClosable(count() > 1);
}
