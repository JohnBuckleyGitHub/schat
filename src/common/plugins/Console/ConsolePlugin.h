/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#ifndef CONSOLEPLUGIN_H_
#define CONSOLEPLUGIN_H_

#include "ChatApi.h"
#include "CoreApi.h"

class ConsolePlugin : public QObject, CoreApi, ChatApi
{
  Q_OBJECT
  Q_INTERFACES(CoreApi ChatApi)

# if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "me.schat.client.ConsolePlugin" FILE "Console.json")
# endif

public:
  QVariantMap header() const
  {
    QVariantMap out        = CoreApi::header();
    out[CORE_API_ID]       = "Console";
    out[CORE_API_NAME]     = "Console";
    out[CORE_API_VERSION]  = "0.2.1";
    out[CORE_API_SITE]     = "http://wiki.schat.me/Plugin/Console";
    out[CORE_API_DESC]     = "Server Management console";
    out[CORE_API_DESC_RU]  = "Консоль управления сервером";
    out[CORE_API_ENABLED]  = false;

    return out;
  }

  ChatPlugin *create();
};

#endif /* CONSOLEPLUGIN_H_ */
