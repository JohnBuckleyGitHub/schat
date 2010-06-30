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

#include "statusmenu.h"

/*!
 * \brief Конструктор класса StatusMenu.
 */
StatusMenu::StatusMenu(QWidget *parent)
  : QMenu(tr("Статус"), parent),
    m_group(new QActionGroup(this))
{
  QAction *action = m_group->addAction(icon(StatusOnline), tr("В сети"));
  action->setShortcut(Qt::CTRL + Qt::Key_1);
  m_statuses.append(action);

  action = m_group->addAction(icon(StatusAway), tr("Отсутствую"));
  action->setShortcut(Qt::CTRL + Qt::Key_2);
  m_statuses.append(action);

  action = m_group->addAction(icon(StatusDnD), tr("Не беспокоить"));
  action->setShortcut(Qt::CTRL + Qt::Key_3);
  m_statuses.append(action);

  action = m_group->addAction(icon(StatusOffline), tr("Не в сети"));
  action->setShortcut(Qt::CTRL + Qt::Key_0);
  m_statuses.append(action);

  for (int i = 0; i < m_statuses.size(); ++i) {
    m_statuses.at(i)->setData(i);
    m_statuses.at(i)->setCheckable(true);
  }

  for (int i = 0; i < 3; ++i) {
    addAction(m_statuses.at(i));
  }

  addSeparator();
  addAction(m_statuses.at(StatusOffline));

  connect(m_group, SIGNAL(triggered(QAction *)), SLOT(statusChanged(QAction *)));
}


/*!
 * Возвращает иконку ассоциированную с выбранным статусом.
 */
QIcon StatusMenu::icon(Status status) const
{
  switch (status) {
    case StatusOnline:
      return QIcon(":/images/status/online.png");

    case StatusAway:
      return QIcon(":/images/status/away.png");

    case StatusDnD:
      return QIcon(":/images/status/dnd.png");

    case StatusOffline:
      return QIcon(":/images/status/offline.png");
  }

  return QIcon();
}


/*!
 * Установка статуса.
 */
void StatusMenu::setStatus(Status status)
{
  m_statuses.at(status)->setChecked(true);
}


/*!
 * Обработка выбора пользователем в меню нового статуса.
 */
void StatusMenu::statusChanged(QAction *action)
{
  emit statusChanged(action->data().toInt());
}
