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

#ifndef POPUPMANAGER_H_
#define POPUPMANAGER_H_

#include <QObject>

class Alert;
class PopupWindow;
class QDesktopWidget;

class PopupManager : public QObject
{
  Q_OBJECT

public:
  PopupManager(QObject *parent = 0);
  ~PopupManager();

private slots:
  void popup(const Alert &alert);
  void windowDestroyed(QObject *obj);

private:
  void layoutWidgets();

  QDesktopWidget *m_desktop;      ///< Виджет для определения геометрии экрана.
  QList<PopupWindow *> m_windows; ///< Список окон.
};

#endif /* POPUPMANAGER_H_ */
