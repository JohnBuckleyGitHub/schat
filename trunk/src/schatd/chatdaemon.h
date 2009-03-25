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

#ifndef CHAT_DAEMON_H_
#define CHAT_DAEMON_H_

#include <QHash>
#include <QObject>

#include "chatuser.h"

class ChatServer;
struct UserData;

/*!
 * \brief Сервер чата
 *
 * Класс полностью включает в себя функциональность сервера чата.
 */
class ChatDaemon : public QObject
{
  Q_OBJECT

public:
  ChatDaemon(QObject *parent = 0);
  ~ChatDaemon();
  inline static ChatDaemon *instance() { return m_self; }

public slots:
  void greeting(const UserData &data);
  void localLeave(const QString &nick);

private slots:
  void start();

private:
  ChatServer *m_server;
  QHash<QString, boost::shared_ptr<ChatUser> > m_users;
  static ChatDaemon *m_self;
};

#endif /*CHAT_DAEMON_H_*/
