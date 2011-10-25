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

#include <QGridLayout>
#include "QCheckBox"
#include "QPushButton"
#include <QAction>
#include <QEvent>

#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "ui/NetworkEditor.h"
#include "ui/NetworkWidget.h"

NetworkEditor::NetworkEditor(QWidget *parent)
  : QWidget(parent)
{
  m_network = new NetworkWidget(this);
  m_anonymous = new QCheckBox(this);
  m_connect = new QPushButton(this);

  QGridLayout *mainLay = new QGridLayout(this);
  mainLay->addWidget(m_network, 0, 0, 1, 2);
  mainLay->addWidget(m_anonymous, 1, 0);
  mainLay->addWidget(m_connect, 1, 1);
  mainLay->setColumnStretch(0, 1);
  mainLay->setMargin(0);
  mainLay->setSpacing(4);

  connect(ChatCore::i()->client(), SIGNAL(clientStateChanged(int, int)), SLOT(clientStateChanged()));
  connect(m_connect, SIGNAL(clicked()), m_network, SLOT(open()));

  retranslateUi();
}


void NetworkEditor::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QWidget::changeEvent(event);
}


void NetworkEditor::clientStateChanged()
{
  QAction *action = m_network->connectAction();
  m_connect->setIcon(action->icon());
  m_connect->setText(action->text());
}


void NetworkEditor::retranslateUi()
{
  m_anonymous->setText(tr("Anonymous connection"));
  clientStateChanged();
}
