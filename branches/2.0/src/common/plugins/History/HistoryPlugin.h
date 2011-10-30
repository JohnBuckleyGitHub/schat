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

#ifndef HISTORYPLUGIN_H_
#define HISTORYPLUGIN_H_

#include "ChatApi.h"
#include "CoreApi.h"

class ChatPlugin;

class HistoryPlugin : public QObject, CoreApi, ChatApi
{
  Q_OBJECT
  Q_INTERFACES(CoreApi ChatApi)

public:
  ChatPlugin *init(ChatCore *core);
  QString id() const { return QLatin1String("History"); }
  QString name() const { return id(); }
  QString version() const { return QLatin1String("1.1.0"); }
  QStringList provides() const { return QStringList(id()); }
};

#endif /* HISTORYPLUGIN_H_ */
