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

#ifndef YOUTUBEPLUGIN_P_H_
#define YOUTUBEPLUGIN_P_H_

#include "plugins/ChatPlugin.h"

class QUrl;

class YouTubePluginImpl : public ChatPlugin
{
  Q_OBJECT

public:
  YouTubePluginImpl(QObject *parent);

private:
};

#endif /* YOUTUBEPLUGIN_P_H_ */
