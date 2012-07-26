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

#ifndef USER_H_
#define USER_H_

#include <QVariant>

#include "schat.h"

class ServerChannel;

class SCHAT_EXPORT Birthday
{
public:
  Birthday(quint8 day = 0, quint8 month = 0, quint16 year = 0);

  quint8 day;
  quint8 month;
  quint16 year;
};

class SCHAT_EXPORT User
{
public:
  User();
  bool set(const QString &key, const QVariant &value);
  QVariantMap toMap() const;
  void toMap(QVariantMap &out) const;

  qint64 date;       ///< Дата модификации.
  QString name;      ///< Имя пользователя.
  QString email;     ///< email адрес.
  QString city;      ///< Город.
  QString country;   ///< Страна.
  Birthday birthday; ///< День рождения.
  QString link;      ///< Ссылка на профиль в социальной сети.
  QString site;      ///< Web-сайт пользователя.
  QVariantMap extra; ///< Дополнительные поля.

private:
  bool setString(QString &key, const QVariant &value);
};

#endif /* USER_H_ */
