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

#include <QLineEdit>

#include "ui/network/SecurityQuestion.h"

SecurityQuestion::SecurityQuestion(QWidget *parent)
  : QComboBox(parent)
{
  addItem(tr("Choose a question ..."));
  addItem(tr("What is the name of your best friend from childhood?"));
  addItem(tr("What was the name of your first teacher?"));
  addItem(tr("What is the name of your manager at your first job?"));
  addItem(tr("What was your first phone number?"));
  addItem(tr("What is your vehicle registration number?"));
  addItem(tr("My own question"));

  connect(this, SIGNAL(currentIndexChanged(int)), SLOT(indexChanged(int)));
}


void SecurityQuestion::indexChanged(int index)
{
  if (index == 6) {
    setEditable(true);
    if (lineEdit())
      lineEdit()->selectAll();
  }
  else
    setEditable(false);
}
