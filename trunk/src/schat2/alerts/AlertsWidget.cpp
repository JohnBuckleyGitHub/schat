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
#include <QComboBox>
#include <QVBoxLayout>

#include "alerts/AlertsWidget.h"
#include "alerts/AlertType.h"
#include "ChatAlerts.h"

AlertsWidget::AlertsWidget(QWidget *parent)
  : QWidget(parent)
{
  m_combo = new QComboBox(this);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(m_combo);
  layout->setMargin(0);

  QList<AlertType*> types = ChatAlerts::types();
  foreach (AlertType *type, types) {
    m_combo->addItem(type->icon(), type->name(), type->type());
  }
}
