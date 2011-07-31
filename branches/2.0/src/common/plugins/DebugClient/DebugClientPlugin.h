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

#ifndef DEBUGCLIENTPLUGIN_H_
#define DEBUGCLIENTPLUGIN_H_

#include "ClientApi.h"
#include "CoreApi.h"

class DebugClient;

class DebugClientPlugin : public QObject, CoreApi, ClientApi
{
  Q_OBJECT
  Q_INTERFACES(CoreApi ClientApi)

public:
  QObject *init(ClientHelper *helper, FileLocations *locations);
  QString id() const { return "DebugClient"; }
  QString name() const { return "Debug Client"; }

private:
  DebugClient *d;
};

#endif /* DEBUGCLIENTPLUGIN_H_ */
