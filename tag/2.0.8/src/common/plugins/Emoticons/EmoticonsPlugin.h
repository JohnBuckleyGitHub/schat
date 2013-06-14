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

#ifndef EMOTICONSPLUGIN_H_
#define EMOTICONSPLUGIN_H_

#include "ChatApi.h"
#include "CoreApi.h"

class EmoticonsPlugin : public QObject, CoreApi, ChatApi
{
  Q_OBJECT
  Q_INTERFACES(CoreApi ChatApi)

# if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "me.schat.client.Emoticons" FILE "Emoticons.json")
# endif

public:
  QVariantMap header() const
  {
    QVariantMap out        = CoreApi::header();
    out[CORE_API_ID]       = "Emoticons";
    out[CORE_API_NAME]     = "Emoticons";
    out[CORE_API_VERSION]  = "0.2.4";
    out[CORE_API_SITE]     = "http://wiki.schat.me/Plugin/Emoticons";
    out[CORE_API_DESC]     = "Adds support for graphical emoticons";
    out[CORE_API_DESC_RU]  = "Добавляет поддержку графических смайликов";

    return out;
  }

  bool check() const;
  ChatPlugin *create();
};

#endif /* EMOTICONSPLUGIN_H_ */
