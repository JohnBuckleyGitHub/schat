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

#ifndef STORAGEHOOK_H_
#define STORAGEHOOK_H_

#include <QStringList>

/*!
 * Базовый класс для хуков хранилища данных сервера.
 */
class StorageHook
{
public:
  StorageHook();
  virtual ~StorageHook() {}

  inline bool cache() const { return m_cache; }
  virtual bool setValue(const QString &key, const QVariant &value);
  virtual QStringList keys() const;
  virtual QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

protected:
  bool m_cache; ///< \b true если значения допускают кеширование.
};

#endif /* STORAGEHOOK_H_ */
