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

#include <QDebug>

#include <QGridLayout>
#include <QLabel>

#include "ui/profile/ProfileLayout.h"
#include "ui/profile/ProfileField.h"
#include "ui/profile/TextField.h"
#include "Profile.h"

ProfileLayout::ProfileLayout(QWidget *parent)
  : QWidget(parent)
{
  m_layout = new QGridLayout(this);
  m_layout->setContentsMargins(10, 10, 3, 0);
}


/*!
 * Добавления поля редактирования профиля.
 *
 * \todo Добавить поддержку фабрики для создания редакторов в плагинах.
 */
void ProfileLayout::add(const QString &field)
{
  if (m_fields.contains(field))
    return;

  int index = Profile::fields.indexOf(field);
  if (index == -1)
    return;

  ProfileField *widget = new TextField(field, this);
  m_fields[field] = widget;

  m_layout->addWidget(widget->label(), index, 0);
  m_layout->addWidget(widget, index, 1);

  qDebug() << "ProfileLayout::add()" << field;


}
