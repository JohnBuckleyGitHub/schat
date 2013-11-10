/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include <QToolButton>
#include <QGridLayout>

#include "ShareWebWidget.h"
#include "ui/SLineEdit.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"

ShareWebWidget::ShareWebWidget(QWidget *parent)
  : QFrame(parent)
{
  m_urlEdit = new SLineEdit(this);
  m_urlEdit->setMinimumWidth(256);

  m_addBtn = new QToolButton(this);
  m_addBtn->setIcon(SCHAT_ICON(Add));
  m_addBtn->setText(tr("Add"));
  m_addBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

  QGridLayout *layout = new QGridLayout(this);
  layout->addWidget(m_urlEdit, 0, 0);
  layout->addWidget(m_addBtn, 0, 1);
  layout->setMargin(6);

  setFocusPolicy(Qt::WheelFocus);
}


void ShareWebWidget::showEvent(QShowEvent *event)
{
  QFrame::showEvent(event);

  m_urlEdit->setFocus();
}
