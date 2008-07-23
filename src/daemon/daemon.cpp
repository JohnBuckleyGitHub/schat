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
#include <QtNetwork>

#include "abstractprofile.h"
#include "channellog.h"
#include "daemon.h"
#include "daemonservice.h"
#include "daemonsettings.h"
#include "log.h"
#include "protocol.h"
#include "userunit.h"


/** [public]
 * 
 */
Daemon::Daemon(QObject *parent)
  : QObject(parent)
{
  m_settings = new DaemonSettings(this);
  connect(&m_server, SIGNAL(newConnection()), SLOT(incomingConnection()));
}


/** [public]
 * Запуск демона, возвращает `true` в случае успешного запуска, и `false` не успешного.
 */
bool Daemon::start()
{
  m_settings->read();
  
  if (m_settings->channelLog) {
    m_channelLog = new ChannelLog(this);
    m_channelLog->setChannel("#main");
    m_channelLog->setMode(ChannelLog::Plain);
  }
  
  if (m_settings->privateLog) {
    m_privateLog = new ChannelLog(this);
    m_privateLog->setChannel("#private");
    m_privateLog->setMode(ChannelLog::Plain);
  }
  
  QString address = m_settings->listenAddress;
  quint16 port    = m_settings->listenPort;
  bool result     = m_server.listen(QHostAddress(address), port);
  
  if (result) {
    LOG(0, tr("Simple Chat Daemon успешно запущен, адрес %1, порт %2").arg(address).arg(port));
  }
  else {
    LOG(0, tr("Ошибка запуска Simple Chat Daemon, %1").arg(m_server.errorString()));
  }
  
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
    connect(this, SIGNAL(newUser(const QStringList &, bool)), service, SLOT(newUser(const QStringList &, bool)));
  }
}


/** [private slots]
 * Слот вызывается сигналом `greeting(const QStringList &)` от сервиса ожидающего
 * проверки приветствия (проверка на дубдикат ников).
 * В случае успеха сервис уведомляется (`accessGranted()`) об этом, добавляется в список пользователей
 * и высылается сигнал `newUser(const QStringList &list)`.
 * В случае дубликата ников высылается код ошибки `ErrorNickAlreadyUse`.
 */
void Daemon::greeting(const QStringList &list)
{
  qDebug() << "Daemon::greeting(const QStringList &)" << list.at(AbstractProfile::Nick);
  
  if (DaemonService *service = qobject_cast<DaemonService *>(sender())) {
    QString nick = list.at(AbstractProfile::Nick);
    
    if (!m_users.contains(nick)) {
      m_users.insert(nick, new UserUnit(list, service));
      service->accessGranted();
      emit newUser(list, true);
      
      LOG(0, tr(">>> (%1), %2, %3, %4, %5")
          .arg(list.at(AbstractProfile::Host))
          .arg(list.at(AbstractProfile::Nick))
          .arg(list.at(AbstractProfile::FullName))
          .arg(list.at(AbstractProfile::Gender))
          .arg(list.at(AbstractProfile::UserAgent)));
      
      if (m_settings->channelLog)
        if (list.at(AbstractProfile::Gender) == "male")
          m_channelLog->msg(tr("`%1` зашёл в чат").arg(nick));
        else
          m_channelLog->msg(tr("`%1` зашла в чат").arg(nick));
    }
    else
      service->accessDenied(ErrorNickAlreadyUse);
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
    
    LOG(0, tr("<<< (%1), %2").arg(unit->profile()->host()).arg(nick));
    
    if (m_settings->channelLog)
      if (unit->profile()->isMale())
        m_channelLog->msg(tr("`%1` вышел из чата: %2").arg(nick).arg(unit->profile()->byeMsg()));
      else
        m_channelLog->msg(tr("`%1` вышла из чата: %2").arg(nick).arg(unit->profile()->byeMsg()));
    
    delete unit;
  }
}
