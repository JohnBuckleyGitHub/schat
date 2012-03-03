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

#include <QEvent>
#include <QLineEdit>

#include "ui/network/SecurityQuestion.h"

SecurityQuestion::SecurityQuestion(QWidget *parent)
  : QComboBox(parent)
{
  addItem(QString());
  addItem(QString());
  addItem(QString());
  addItem(QString());
  addItem(QString());
  addItem(QString());
  addItem(QString());

  connect(this, SIGNAL(currentIndexChanged(int)), SLOT(indexChanged(int)));

  retranslateUi();
}


void SecurityQuestion::changeEvent(QEvent *event)
{
  if (event->type() == QEvent::LanguageChange)
    retranslateUi();

  QComboBox::changeEvent(event);
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


void SecurityQuestion::retranslateUi()
{
  setItemText(0, tr("Choose a question ..."));
  setItemText(1, tr("What is the name of your best friend from childhood?"));
  setItemText(2, tr("What was the name of your first teacher?"));
  setItemText(3, tr("What is the name of your manager at your first job?"));
  setItemText(4, tr("What was your first phone number?"));
  setItemText(5, tr("What is your vehicle registration number?"));
  setItemText(6, tr("My own question"));
}
