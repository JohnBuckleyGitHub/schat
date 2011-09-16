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

#ifndef MESSAGELOGPLUGIN_H_
#define MESSAGELOGPLUGIN_H_

#include "CoreApi.h"
#include "NodeApi.h"

class NodePlugin;

class MessageLogPlugin : public QObject, CoreApi, NodeApi
{
  Q_OBJECT
  Q_INTERFACES(CoreApi NodeApi)

public:
  NodePlugin *init(Core *core);
  QString id() const { return QLatin1String("MessageLog"); }
  QString name() const { return QLatin1String("Message Log"); }
  QString version() const { return QLatin1String("1.0.0"); }
};

#endif /* MESSAGELOGPLUGIN_H_ */
