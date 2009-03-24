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

#include <QtCore>
#include <QtNetwork>

#include "chatdaemon.h"
#include "chatserver.h"
#include "packet.h"
#include "schatd.h"

ChatDaemon *ChatDaemon::m_self = 0;

/*!
 * \brief Конструктор класса ChatDaemon.
 * \param parent Указатель на родительский объект.
 */
ChatDaemon::ChatDaemon(QObject *parent)
  : QObject(parent)
{
  qDebug() << this;

  Q_ASSERT(!m_self);
  m_self = this;

  QTimer::singleShot(0, this, SLOT(start()));
}


ChatDaemon::~ChatDaemon()
{
  qDebug() << "~" << this;
}


void ChatDaemon::packet(const UserData &data)
{
//  qDebug() << this << "packet()" << data.nick;

  Connection *con = qobject_cast<Connection *>(sender());
  if (con) {
    con->ready();
    con->send(Packet::create(OpcodeAccessGranted, 0));
  }

//  ChatUser *user = new ChatUser(data, qobject_cast<Connection *>(sender()));
//  m_users.insert(data.nick, user);

//  Connection *connection = qobject_cast<Connection *>(sender());
//  if (connection)
//    connection->send(Packet::create(OpcodeAccessDenied, 999));
}


/*!
 * Запуск сервера.
 *
 * Функция читает настройки сервера, устанавливает параметры логирования каналов
 * и производит запуск сервера \a m_server.
 * Данные о попытке запуска заносятся в лог файл.
 */
void ChatDaemon::start()
{
//  qDebug() << thread();
  qDebug() << this << "start()";
  m_server = new ChatServer("0.0.0.0", 7777, 2);
  m_server->start();
}
