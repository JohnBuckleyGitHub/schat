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

#include <QDate>
#include <QLocale>
#include <QObject>
#include <QStringList>

#include <QVariant>

#include "version.h"

/*!
 * Базовый интерфейс для всех типов плагинов.
 */
class CoreApi
{
public:
  virtual ~CoreApi() {}

  /// Получение информации о плагине.
  virtual QVariantMap header() const
  {
    QVariantMap out;
    out["Author"]  = "IMPOMEZIA";       // Автор плагина.
    out["Id"]      = "";                // Машинное имя плагина.
    out["Name"]    = "";                // Имя плагина.
    out["Version"] = "0.1.0";           // Версия плагина.
    out["Site"]    = "http://schat.me"; // Домашняя страница плагина.
    out["Desc"]    = "";                // Описание плагина.

    return out;
  }
};

Q_DECLARE_INTERFACE(CoreApi, "me.schat.CoreApi/1.1");

#endif /* COREINTERFACE_H_ */
