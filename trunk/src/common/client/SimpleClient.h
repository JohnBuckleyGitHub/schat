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

#ifndef SIMPLECLIENT_H_
#define SIMPLECLIENT_H_

#include "client/AbstractClient.h"

class AbstractNotice;
class SimpleClientPrivate;

class AuthError
{
public:
  AuthError()
  : authType(-1)
  , status(-1)
  {}

  AuthError(int authType, int status)
  : authType(authType)
  , status(status)
  {}

  int authType;
  int status;
};


class SCHAT_EXPORT SimpleClient : public AbstractClient
{
  Q_OBJECT

public:
  ///< Данные пользователя которые были изменены.
  enum UserChangeData {
    UserNotChanged = 0,        ///< Данные пользователя не изменены.
    UserBasicDataChanged = 1,  ///< Изменены основные данные пользователя.
    UserNickChanged = 2,       ///< Изменён ник пользователя.
    UserStaticDataChanged = 4, ///< Изменены статические данные пользователя.
    UserStatusChanged = 8,     ///< Изменён текстовый статус пользователя.
    JSonDataChanged = 16,
    UserCompletelyChanged = UserBasicDataChanged | UserNickChanged | UserStaticDataChanged | UserStatusChanged | JSonDataChanged ///< Данные пользователя полностью изменены.
  };

  explicit SimpleClient(QObject *parent = 0);
  ~SimpleClient();
  ClientChannel channel() const;
  ClientChannel channel(const QByteArray &id) const;
  const AuthError& authError() const;
  const QString &account() const;
  void leave();
  void setAccount(const QString &account, const QString &password);
  void setCookieAuth(bool allow);

signals:
  void message(const MessageData &data);
  void notice(const Notice &notice);
  void notice(int type);
  void split(const QList<QByteArray> &users);
  void userLeave(const QByteArray &userId);

protected:
  SimpleClient(SimpleClientPrivate &dd, QObject *parent);

protected slots:
  void requestAuth();

protected:
  void newPacketsImpl();

private:
  Q_DECLARE_PRIVATE(SimpleClient);
};

#endif /* SIMPLECLIENT_H_ */