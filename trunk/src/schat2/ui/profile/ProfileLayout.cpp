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

#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QTimer>
#include <QToolButton>

#include "Profile.h"
#include "sglobal.h"
#include "ui/ChatIcons.h"
#include "ui/profile/ProfileField.h"
#include "ui/profile/ProfileLayout.h"
#include "ui/profile/TextField.h"

ProfileLayout::ProfileLayout(QWidget *parent)
  : QWidget(parent)
{
  m_menu = new QMenu(this);
  m_button = new QToolButton(this);
  m_button->setMenu(m_menu);
  m_button->setText(tr("Add") + LS(" "));
  m_button->setIcon(SCHAT_ICON(Add));
  m_button->setPopupMode(QToolButton::InstantPopup);
  m_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

  m_layout = new QGridLayout(this);
  m_layout->setContentsMargins(10, 10, 3, 0);

  connect(m_menu, SIGNAL(aboutToShow()), SLOT(menuAboutToShow()));
  connect(m_menu, SIGNAL(triggered(QAction *)), SLOT(menuTriggered(QAction *)));

  QTimer::singleShot(0, this, SLOT(reload()));
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

  m_button->setVisible(!available().isEmpty());
}


void ProfileLayout::menuAboutToShow()
{
  m_menu->clear();
  QStringList fields = available();
  if (fields.isEmpty())
    return;

  foreach (QString field, fields) {
    QAction *action = m_menu->addAction(Profile::translate(field));
    action->setData(field);
  }
}


void ProfileLayout::menuTriggered(QAction *action)
{
  if (action)
    add(action->data().toString());
}


void ProfileLayout::reload()
{
  QStringList filled = Profile::filled();
  foreach (QString field, filled) {
    add(field);
  }
}


QStringList ProfileLayout::available() const
{
  QStringList out = Profile::available();
  foreach (QString field, m_fields.keys()) {
    out.removeAll(field);
  }

  return out;
}
