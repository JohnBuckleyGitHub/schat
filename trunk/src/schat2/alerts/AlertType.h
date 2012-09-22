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

#ifndef ALERTTYPE_H_
#define ALERTTYPE_H_

#include <QIcon>
#include <QVariant>

/*!
 * Базовый класс, хранящий информацию об обработке уведомления.
 */
class AlertType
{
public:
  AlertType(const QString &type, int weight);
  virtual ~AlertType() {}
  inline bool popup() const           { return value(QLatin1String("popup"), false).toBool(); }
  inline bool tray() const            { return value(QLatin1String("tray"), false).toBool(); }
  inline const QIcon& icon() const    { return m_icon; }
  inline const QString& type() const  { return m_type; }
  inline int weight() const           { return m_weight; }
  inline QString sound() const        { return value(QLatin1String("sound"), QString()).toString(); }
  inline virtual QString name() const { return m_type; }
  QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

  QVariantMap options;    ///< Выбранные опции.

protected:
  int m_weight;           ///< Вес уведомления, используется для сортировки.
  QIcon m_icon;           ///< Иконка уведомления.
  QString m_type;         ///< Тип уведомления.
  QVariantMap m_defaults; ///< Параметры поддерживаемых типов уведомлений по умолчанию.
};

#endif /* ALERTTYPE_H_ */
