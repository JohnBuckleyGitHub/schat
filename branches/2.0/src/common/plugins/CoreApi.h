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

/*!
 * Базовый интерфейс для всех типов плагинов.
 */
class CoreApi
{
public:
  virtual ~CoreApi() {}
  virtual QString author() const       { return QLatin1String("IMPOMEZIA"); }
  virtual QString description() const  { return QString(); }
  virtual QString id() const           = 0;
  virtual QString name() const         = 0;
  virtual QString site() const         { return QLatin1String("http://impomezia.com"); }
  virtual QString version() const      { return QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy")).toString(QLatin1String("yyyy.MM.dd")); }
  virtual QStringList provides() const { return QStringList(); }
  virtual QStringList required() const { return QStringList(); }
};

Q_DECLARE_INTERFACE(CoreApi, "me.schat.CoreApi/1.0");

#endif /* COREINTERFACE_H_ */
