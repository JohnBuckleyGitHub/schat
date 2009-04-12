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

#ifndef SERVICEINSTALLER_H_
#define SERVICEINSTALLER_H_

#include <QObject>

/*!
 * \brief Устанавливает и удаляет Win32 сервис.
 */
class ServiceInstaller : public QObject
{
  Q_OBJECT

public:
  ServiceInstaller(QObject *parent = 0);
  ~ServiceInstaller();

  static bool exists(const QString &name);
};


#endif /* SERVICEINSTALLER_H_ */
