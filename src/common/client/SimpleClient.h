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
  ClientChannel channel(const QByteArray &id) const;
  ClientUser user() const;
  ClientUser user(const QByteArray &id) const;
  void leave();
  void part(const QByteArray &channelId);
  void setAccount(const QString &account, const QString &password);
  void setCookieAuth(bool allow);

signals:
  void join(const QByteArray &channelId);
  void join(const QByteArray &channelId, const QByteArray &userId);
  void message(const MessageData &data);
  void notice(const Notice &notice);
  void part(const QByteArray &channelId, const QByteArray &userId);
  void split(const QList<QByteArray> &users);
  void synced(const QByteArray &channelId);
  void userDataChanged(const QByteArray &userId, int changed = 0);
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
