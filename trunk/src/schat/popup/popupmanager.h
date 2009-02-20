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

#ifndef POPUPMANAGER_H_
#define POPUPMANAGER_H_

#include <QObject>

/*!
 * \brief Менеджер всплывающих окон.
 */
class PopupManager : public QObject
{
  Q_OBJECT

public:
  PopupManager(QObject *parent = 0);
  ~PopupManager();

private:
  class Private;
  Private *const d;
};

#endif /* POPUPMANAGER_H_ */
