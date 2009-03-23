/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#ifndef USERTOOLS_H_
#define USERTOOLS_H_

#include <QString>

class UserTools {
  enum {
    MaxNickSize      = 24,
    MinNickSize      = 2,
    MaxNameSize      = 120,
    MinNameSize      = 0,
    MaxByeMsgSize    = 100,
    MinByeMsgSize    = 0,
    MaxUserAgentSize = 100,
    MinUserAgentSize = 10
  };

  UserTools() {}

public:
  inline static QString byeMsg(const QString &msg)        { return msg.simplified().left(MaxByeMsgSize); }
  inline static QString fullName(const QString &fullName) { return fullName.simplified().left(MaxNameSize); }
  inline static QString nick(const QString &nick)         { return nick.simplified().left(MaxNickSize); }
  inline static QString userAgent(const QString &agent)   { return agent.simplified().left(MaxUserAgentSize); }
  static bool isValidNick(const QString &nick);
  static bool isValidUserAgent(const QString &agent);
};

#endif /* USERTOOLS_H_ */
