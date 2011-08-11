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

#ifndef SLAVENODEPLUGIN_H_
#define SLAVENODEPLUGIN_H_

#include "CoreApi.h"
#include "NodeKernelApi.h"

class SlaveNodePlugin : public QObject, CoreApi, NodeKernelApi
{
  Q_OBJECT
  Q_INTERFACES(CoreApi NodeKernelApi)

public:
  Core *init();
  QString id() const { return QLatin1String("SlaveNode"); }
  QString name() const { return QLatin1String("Slave Node Kernel"); }
  QStringList provides() const { return QStringList(id()); }
};

#endif /* SLAVENODEPLUGIN_H_ */
