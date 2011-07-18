/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#ifndef COREINTERFACE_H_
#define COREINTERFACE_H_

#include <QObject>
#include <QStringList>

/*!
 * Базовый интерфейс для всех типов плагинов.
 */
class CoreInterface
{
public:
  virtual ~CoreInterface() {}
  virtual QString description() const { return ""; }
  virtual QString id() const = 0;   ///< Машинное имя плагина.
  virtual QString name() const = 0; ///< Имя плагина.
  virtual QString version() const { return ""; }
  virtual QStringList provides() const { return QStringList(); }
  virtual QStringList required() const { return QStringList(); }
};

Q_DECLARE_INTERFACE(CoreInterface, "com.impomezia.schat.CoreInterface/1.0");

#endif /* COREINTERFACE_H_ */
