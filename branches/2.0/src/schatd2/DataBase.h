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

#ifndef DATABASE_H_
#define DATABASE_H_

#include <QObject>

#include "ServerChannel.h"
#include "ServerUser.h"

/*!
 * Информация об аккаунте пользователя.
 */
class SCHAT_EXPORT Account
{
public:
  Account()
  : id(-1)
  {}

  bool isValid() const;

  qint64 id;
  QByteArray userId;
  QString name;
  QByteArray password;
  QVariant data;
};


/*!
 * База данных сервера.
 */
class SCHAT_EXPORT DataBase : public QObject
{
  Q_OBJECT

public:
  DataBase(QObject *parent = 0);
  int start();

  // users.
  ChatUser user(const QByteArray &id);
  ChatUser user(qint64 id);
  qint64 add(ChatUser user);
  qint64 addGroup(const QString &name, const QString &permissions = QString());
  qint64 userKey(const QByteArray &id);
  void update(ChatUser user);

  // channels.
  ChatChannel channel(const QByteArray &id);
  ChatChannel channel(qint64 id);
  qint64 add(ChatChannel channel);
  qint64 channelKey(const QByteArray &id);
  void update(ChatChannel channel);

  // accounts.
  Account account(const QString &name) const;
  qint64 reg(ChatUser user, const QString &name, const QByteArray &password, const QVariant &data);
};

#endif /* DATABASE_H_ */
