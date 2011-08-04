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

#ifndef ABSTRACTHISTORY_H_
#define ABSTRACTHISTORY_H_

#include "plugins/ChatPlugin.h"
#include "User.h"

class SCHAT_CORE_EXPORT AbstractHistory : public ChatPlugin
{
  Q_OBJECT

public:
  AbstractHistory(ChatCore *core);
  virtual ClientUser user(const QByteArray &id) const { return ClientUser(); }
};

#endif /* ABSTRACTHISTORY_H_ */
