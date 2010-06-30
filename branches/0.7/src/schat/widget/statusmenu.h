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

#ifndef STATUSMENU_H_
#define STATUSMENU_H_

#include <QMenu>
#include <QObject>

/*!
 * \brief Обеспечивает функциональность меню выбора смайликов.
 */
class StatusMenu : public QMenu
{
  Q_OBJECT

public:
  enum Status {
    StatusOnline,
    StatusAway,
    StatusDnD,
    StatusOffline
  };

  StatusMenu(QWidget *parent = 0);
  QIcon icon(Status status) const;
  void setStatus(Status status);

signals:
  void statusChanged(int status);

private slots:
  void statusChanged(QAction *action);

private:
  QActionGroup *m_group;       ///< Группа для того чтобы можно было выбрать только один статус.
  QList<QAction *> m_statuses; ///< Список статусов.
};

#endif /* STATUSMENU_H_ */
