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

#include <QDebug>
#include <QToolButton>
#include <QToolBar>

#include "ChatCore.h"
#include "ChatSettings.h"
#include "ui/fields/NickEdit.h"
#include "ui/UserUtils.h"
#include "User.h"

NickEdit::NickEdit(QWidget *parent)
  : ProfileField(ChatSettings::ProfileNick, parent)
{
  init();
}


NickEdit::NickEdit(const QString &contents, QWidget *parent)
  : ProfileField(ChatSettings::ProfileNick, contents, parent)
{
  init();
}


void NickEdit::init()
{
  setMaxLength(User::MaxNickLength);

  m_toolBar = new QToolBar(this);
  m_toolBar->setIconSize(QSize(16, 16));

  #if defined(Q_OS_MAC)
  m_toolBar->setStyleSheet("QToolBar { margin:0px; border:0px; }");
  #endif

  m_color = new QToolButton(this);
  m_color->setIcon(QIcon(":/images/user.png"));
  m_color->setCursor(Qt::ArrowCursor);

  m_config = new QToolButton(this);
  m_config->setIcon(SCHAT_ICON(GearIcon));
  m_config->setCursor(Qt::ArrowCursor);

  m_toolBar->addWidget(m_color);
  m_toolBar->addWidget(m_config);

  addWidget(m_toolBar, RightSide);
}
