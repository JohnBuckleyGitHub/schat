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

#ifndef CHATAPI_H_
#define CHATAPI_H_

#include <QObject>

class ChatCore;
class ChatPlugin;

class ChatApi
{
public:
  virtual ~ChatApi() {}
  virtual ChatPlugin *init(ChatCore *core) = 0;
};

Q_DECLARE_INTERFACE(ChatApi, "com.impomezia.schat.ChatApi/1.0");

#endif /* CHATAPI_H_ */
