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
  setStyleSheet(QString("ChannelBar { background-color:%1; margin:0px; border:0px; }").arg(palette().color(QPalette::Base).name()));

  m_topic = new ChannelTopic(this);
  addWidget(m_topic);
}



ChannelTopic::ChannelTopic(QWidget *parent, Qt::WindowFlags f)
  : QLabel(parent, f)
{
  init();
}


ChannelTopic::ChannelTopic(const QString &text, QWidget *parent, Qt::WindowFlags f)
  : QLabel(text, parent, f)
{
  init();
}


void ChannelTopic::init()
{
  setMargin(1);
  setTextFormat(Qt::RichText);
  setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
  setOpenExternalLinks(true);
  setWordWrap(true);
  setSizePolicy(QSizePolicy::Expanding, sizePolicy().verticalPolicy());
}
