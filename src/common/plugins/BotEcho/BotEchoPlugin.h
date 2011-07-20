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

#ifndef BOTECHOPLUGIN_H_
#define BOTECHOPLUGIN_H_

#include "ClientInterface.h"
#include "CoreInterface.h"

class BotEchoPlugin : public QObject, CoreInterface, ClientInterface
{
  Q_OBJECT
  Q_INTERFACES(CoreInterface ClientInterface)

public:
  QObject *init(ClientHelper *helper, FileLocations *locations) { return 0; }
  QString id() const { return "BotEcho"; }
  QString name() const { return "Bot Echo"; }
};

#endif /* BOTECHOPLUGIN_H_ */
