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

class QThreadPool;
class QRunnable;

/*!
 * База данных сервера.
 */
class SCHAT_EXPORT DataBase : public QObject
{
  Q_OBJECT

public:
  DataBase(QObject *parent = 0);
  static int start();

  // users.
  static qint64 addGroup(const QString &name, const QString &permissions = QString());

  // channels.
  static bool isCollision(const QByteArray &id, const QByteArray &normalized, int type);
  static ChatChannel channel(const QByteArray &id, int type = SimpleID::ChannelId);
  static ChatChannel channel(qint64 id);
  static qint64 add(ChatChannel channel);
  static qint64 channelKey(const QByteArray &id, int type = SimpleID::ChannelId);
  static QString nick(qint64 id);
  static void saveData(Channel *channel);
  static void update(ChatChannel channel);

  // accounts.
  static Account account(qint64 key);
  static qint64 accountKey(const QByteArray &cookie);
  static qint64 accountKey(const QString &name);
  static qint64 accountKey(qint64 channel);
  static qint64 add(Account *account);

  static QHash<QByteArray, HostInfo> hosts(qint64 channel);
  static void add(HostInfo host);

  static bool noMaster; /// \b true если отсутвует Master пользователь, значение устанавливается в \b true если таблицы channels не существовало и она была создана.

private slots:
  void startTasks();

private:
  static void version();

  QList<QRunnable*> m_tasks; ///< Задачи для выполнения в отдельном потоке.
  QThreadPool *m_pool;       ///< Пул для запуска потоков.
  static DataBase *m_self;   ///< Указатель на себя.
};

#endif /* DATABASE_H_ */
