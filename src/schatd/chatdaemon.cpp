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


/*!
 * Проверка на наличие локального пользователя.
 *
 * \param nick Ник пользователя.
 *
 * \return \a true Если пользователь уже подключен, непосредственно к данному серверу,
 * во всех остальных случаях \a false, даже если пользователь есть в списке, но подключен
 * к другому серверу.
 *
 * \note Эта функция потокобезопастна.
 */
bool ChatDaemon::isLocalUser(const QString &nick) const
{
  bool result = false;

  m_lock.lockForWrite();
  if (m_users.contains(nick))
    result = m_users.value(nick)->isLocal();

  m_lock.unlock();
  return result;
}


/*!
 * Обработка приветствия от локального пользователя.
 */
void ChatDaemon::greeting(const UserData &data)
{
//  qDebug() << this << "packet()" << data.nick;

  Connection *tmp = qobject_cast<Connection *>(sender());
  if (!tmp)
    return;

  boost::shared_ptr<Connection> connection = tmp->shared_from_this();

  if (m_users.contains(data.nick)) {
    connection->send(Packet::create(OpcodeAccessDenied, ErrorNickAlreadyUse));
    connection->close();
    return;
  }

  boost::shared_ptr<ChatUser> user(new ChatUser(data, connection));
  connect(this, SIGNAL(relayV3(const QByteArray &)), user.get(), SLOT(relay(const QByteArray &)));

  m_lock.lockForRead();
  m_users.insert(data.nick, user);
  m_lock.unlock();
  connection->ready();
  connection->send(Packet::create(OpcodeAccessGranted, 0));

  if (data.protocol == 3) {
    emit relayV3(Packet::create(OpcodeNewUser, 1, 0, data));
  }
}


/*!
 * Обработка отключения локального пользователя.
 */
void ChatDaemon::localLeave(const QString &nick)
{
//  qDebug() << "[1]" << QThread::currentThread() << nick;

  if (m_users.contains(nick)) {
    m_lock.lockForRead();
    m_users.remove(nick);
    m_lock.unlock();
  }
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
