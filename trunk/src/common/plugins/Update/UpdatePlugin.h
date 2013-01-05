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

#ifndef UPDATEPLUGIN_H_
#define UPDATEPLUGIN_H_

#include "ChatApi.h"
#include "CoreApi.h"

class UpdatePlugin : public QObject, CoreApi, ChatApi
{
  Q_OBJECT
  Q_INTERFACES(CoreApi ChatApi)

# if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "me.schat.client.Update" FILE "Update.json")
# endif

public:
  QVariantMap header() const
  {
    QVariantMap out = CoreApi::header();
    out["Id"]           = "Update";
    out["Name"]         = "Update";
    out["Version"]      = "0.3.0";
    out["Site"]         = "http://wiki.schat.me/Plugin/Update";
    out["Desc"]         = "Automatically downloads and installs updates";
    out["Desc/ru"]      = "Автоматически загружает и устанавливает обновления";

#   if defined(Q_OS_WIN)
    out["Configurable"] = true;
#   endif

    return out;
  }

  bool check() const;
  ChatPlugin *create();
  QWidget *settings(QWidget *parent);
};

#endif /* UPDATEPLUGIN_H_ */
