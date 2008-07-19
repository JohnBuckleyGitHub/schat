/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore>

#include "abstractprofile.h"
#include "daemon.h"
#include "daemonservice.h"
#include "protocol.h"
#include "userunit.h"


/** [public]
 * 
 */
Daemon::Daemon(QObject *parent)
  : QObject(parent)
{
  connect(&m_server, SIGNAL(newConnection()), SLOT(incomingConnection()));
}


/** [public]
 * Запуск демона, возвращает `true` в случае успешного запуска, и `false` не успешного.
 */
bool Daemon::start()
{
  QString address = "0.0.0.0"; // FIXME эти данные должны браться из настроек
  quint16 port    = 7667;  
  bool result     = m_server.listen(QHostAddress(address), port);
  
  return result;
}


/** [public slots]
 * Слот вызывается сигналом `newConnection()` от объекта `QTcpServer m_server`.
 * При наличии ожидающих соединений создаётся сервис `DaemonService` для обслуживания клинета. * 
 */
void Daemon::incomingConnection()
{
  if (m_server.hasPendingConnections()) {
    DaemonService *service = new DaemonService(m_server.nextPendingConnection(), this);
    connect(service, SIGNAL(greeting(const QStringList &)), SLOT(greeting(const QStringList &)));
    connect(service, SIGNAL(leave(const QString &)), SLOT(userLeave(const QString &)));
    connect(this, SIGNAL(newUser(const QStringList &)), service, SLOT(newUser(const QStringList &)));
  }
}


/** [private slots]
 * Слот вызывается сигналом `greeting(const QStringList &)` от сервиса ожидающего
 * проверки приветствия (проверка на дубдикат ников).
 * В случае успеха сервис уведомляется об этом, добавляется в список пользователей
 * и высылается сигнал `newUser(const QStringList &list)`.
 */
void Daemon::greeting(const QStringList &list)
{
  qDebug() << "Daemon::greeting(const QStringList &)" << list.at(AbstractProfile::Nick);
  
  if (DaemonService *service = qobject_cast<DaemonService *>(sender())) {
    QString nick = list.at(AbstractProfile::Nick);
    
    if (!m_users.contains(nick)) {
      m_users.insert(nick, new UserUnit(list, service));
      service->accessGraded();
      emit newUser(list);
    }
    else
      service->accessDenied(sChatErrorNickAlreadyUse);
  }  
}
 

/** [private slots]
 * Слот вызывается сигналом `leave(const QString &)` из сервиса получившего авторизацию
 * перед удалением.
 * Пользователь удаляется из списка пользователей и объект `UserUnit` уничтожается.
 */
void Daemon::userLeave(const QString &nick)
{
  qDebug() << "Daemon::userLeave(const QString &)" << nick;
  
  if (m_users.contains(nick)) {
    UserUnit *unit = m_users.value(nick);
    m_users.remove(nick);
    delete unit;
  }
}
