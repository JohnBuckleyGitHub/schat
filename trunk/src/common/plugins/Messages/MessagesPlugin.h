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

#ifndef MESSAGESPLUGIN_H_
#define MESSAGESPLUGIN_H_

#include "CoreApi.h"
#include "NodeApi.h"

class MessagesPlugin : public QObject, CoreApi, NodeApi
{
  Q_OBJECT
  Q_INTERFACES(CoreApi NodeApi)

public:
  QVariantMap header() const
  {
    QVariantMap out = CoreApi::header();
    out["Id"]       = "Messages";
    out["Name"]     = "Messages";
    out["Version"]  = "0.2.1";
    out["Type"]     = "server";
    out["Site"]     = "http://wiki.schat.me/Plugin/Messages";
    out["Desc"]     = "Server Messages Support";
    out["Required"] = "1.99.27";

    return out;
  }

  NodePlugin *create();
};

#endif /* MESSAGESPLUGIN_H_ */
