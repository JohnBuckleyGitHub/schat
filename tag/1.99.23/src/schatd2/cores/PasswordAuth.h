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

#ifndef PASSWORDAUTH_H_
#define PASSWORDAUTH_H_

#include "cores/CookieAuth.h"

class SCHAT_EXPORT PasswordAuth : public CookieAuth
{
public:
  PasswordAuth(Core *core);
  AuthResult auth(const AuthRequest &data);
  int type() const;

  static QString name(const QString &name);
};

#endif /* PASSWORDAUTH_H_ */
