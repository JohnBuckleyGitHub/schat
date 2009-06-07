/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include <QtGui>

#include "networkwidget.h"
#include "nickedit.h"
#include "quickactions.h"

/*!
 * Конструктор класса QuickActions.
 */
QuickActions::QuickActions(QWidget *parent)
  : QWidget(parent)
{
  NickEdit *nickEdit = new NickEdit(this, NickEdit::GenderButton | NickEdit::ApplyButton);
  NetworkWidget *networkWidget = new NetworkWidget(this);

  QVBoxLayout *mainLay = new QVBoxLayout(this);
  mainLay->addWidget(nickEdit);
  mainLay->addWidget(networkWidget);
  mainLay->setContentsMargins(2, 0, 2, 0);
  mainLay->setSpacing(2);
}


void QuickActions::closePopup()
{
  QMenu *popup = qobject_cast<QMenu *>(parentWidget());
  if (isVisible() && popup)
    popup->close();
}
