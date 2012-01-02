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

#ifndef DATABASE_H_
#define DATABASE_H_

#include <QObject>

#include "Account.h"
#include "net/SimpleID.h"
#include "ServerChannel.h"

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
  qint64 addGroup(const QString &name, const QString &permissions = QString());
  qint64 userKey(const QByteArray &id);

  // channels.
  static ChatChannel channel(const QByteArray &id, int type = SimpleID::ChannelId);
  static ChatChannel channel(qint64 id);
  static qint64 add(ChatChannel channel);
  static qint64 channelKey(const QByteArray &id, int type = SimpleID::ChannelId);
  static void saveData(Channel *channel);
  static void update(ChatChannel channel);

  // accounts.
  static Account account(qint64 key);
  static qint64 accountKey(const QByteArray &cookie);
  static qint64 accountKey(const QString &name);
  static qint64 accountKey(qint64 channel);
  static qint64 add(Account *account);
};

#endif /* DATABASE_H_ */
