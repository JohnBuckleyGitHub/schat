/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>

#include "profilewidget.h"
#include "widget/networkwidget.h"
#include "widget/welcome.h"

WelcomeWidget::WelcomeWidget(QWidget *parent)
  : QWidget(parent)
{
  m_profile = new ProfileWidget(false, this);
  m_network = new NetworkWidget(this, NetworkWidget::NetworkLabel);
  m_connect = new QPushButton(QIcon(":/images/dialog-ok.png"), tr("Connect"), this);
  m_connect->setDefault(true);

  m_ask = new QCheckBox(tr("Всегда использовать это подключение"), this);

  m_grid = new QGridLayout(this);
  m_grid->addWidget(m_profile, 0, 0, 1, 2);
  m_grid->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 2, 1, 1);
  m_grid->addWidget(m_network, 1, 0, 1 ,2);
  m_grid->addWidget(m_connect, 2, 1);
  m_grid->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), 3, 1, 1, 3);
  m_grid->addWidget(m_ask, 4, 0, 1, 3);
  m_grid->setColumnStretch(0, 1);
  m_grid->setColumnStretch(2, 2);
}
