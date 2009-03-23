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

#ifndef SCHATD_H_
#define SCHATD_H_

#include <QMetaType>

/*!
 * Информация об пользователе для обработки подключени.
 */
struct GreetingData {
  QString nick;
  QString fullName;
  QString userAgent;
  QString byeMsg;
  quint8 gender;
  int protocol;
};

Q_DECLARE_METATYPE(GreetingData)

#endif /* SCHATD_H_ */
