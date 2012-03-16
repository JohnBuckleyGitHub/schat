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

#ifndef SIMPLECLIENT_P_H_
#define SIMPLECLIENT_P_H_

#include "client/AbstractClient_p.h"
#include "client/SimpleClient.h"

class PacketReader;
class SyncChannelCache;
class UserReader;

class SimpleClientPrivate : public AbstractClientPrivate
{
  Q_DECLARE_PUBLIC(SimpleClient);

public:
  SimpleClientPrivate();
  virtual ~SimpleClientPrivate();

  bool authReply(const AuthReply &reply);
  bool isFatalError(int status) const;
  bool notice();

  bool cookieAuth;       ///< true если разрешена Сookie авторизация.
  int authType;          ///< Тип авторизации.
  QByteArray password;   ///< Пароль.
  QString account;       ///< Имя аккаунта пользователя.
  QVariantMap json;      ///< Дополнительные данные.
};

#endif /* SIMPLECLIENT_P_H_ */
