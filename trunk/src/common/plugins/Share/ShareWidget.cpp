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

#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QVBoxLayout>

#include "sglobal.h"
#include "ShareWidget.h"
#include "ui/ChatIcons.h"

ShareWidget::ShareWidget(QWidget *parent)
  : QFrame(parent)
{
  m_addLabel = new QLabel(LS("<b>") + tr("Add images from") + LS("</b>"), this);

  m_diskBtn = new QToolButton(this);
  m_diskBtn->setText(tr("Computer"));
  m_diskBtn->setIcon(QIcon(LS(":/images/Share/folder-open.png")));
  m_diskBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  m_diskBtn->setToolTip(tr("Add images from computer"));

  m_webBtn = new QToolButton(this);
  m_webBtn->setText(tr("Web"));
  m_webBtn->setIcon(SCHAT_ICON(Globe));
  m_webBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  m_webBtn->setToolTip(tr("Add images from web"));

  QHBoxLayout *btnLay = new QHBoxLayout();
  btnLay->addWidget(m_diskBtn);
  btnLay->addWidget(m_webBtn);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(m_addLabel);
  layout->addLayout(btnLay);
  layout->setMargin(6);

  connect(m_diskBtn, SIGNAL(clicked()), SLOT(close()));
  connect(m_diskBtn, SIGNAL(clicked()), SIGNAL(addFromDisk()));
  connect(m_webBtn, SIGNAL(clicked()), SLOT(close()));
  connect(m_webBtn, SIGNAL(clicked()), SIGNAL(addFromWeb()));
}


void ShareWidget::close()
{
  QMenu *popup = qobject_cast<QMenu *>(parentWidget());
  if (popup && isVisible())
    popup->close();

  QFrame::close();
}
