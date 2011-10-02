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

#include "ui/InputWidget.h"
#include "ui/tabs/ChannelBar.h"

ChannelBar::ChannelBar(const QString &title, QWidget *parent)
  : QToolBar(title, parent)
{
  init();
}


ChannelBar::ChannelBar(QWidget *parent)
  : QToolBar(parent)
{
  init();
}


void ChannelBar::init()
{
  setIconSize(QSize(10, 10));
  setObjectName(QLatin1String("ChannelBar"));
  setStyleSheet(QString("ChannelBar { background-color:%1; margin:0px; border:0px; }").arg(palette().color(QPalette::Window).name()));

  m_topic = new InputWidget(this);
  m_topic->setEmptySend(true);
  addWidget(m_topic);
}
