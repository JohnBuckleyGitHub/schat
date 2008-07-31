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
#include "clientservice.h"
#include "daemon.h"
#include "daemonservice.h"
#include "daemonsettings.h"
#include "log.h"
#include "protocol.h"
#include "userunit.h"
#include "version.h"

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
  
  if (m_settings->getInt("LogLevel") > -1) {
    m_log = new Log(this);
    if (!m_log->init()) {
      m_log->deleteLater();
      m_settings->setInt("LogLevel", -1);
    }
  }
  
  if (m_settings->getBool("ChannelLog")) {
    m_channelLog = new ChannelLog(this);
    m_channelLog->setChannel("#main");
    m_channelLog->setMode(ChannelLog::Plain);
  }
  
  if (m_settings->getBool("PrivateLog")) {
    m_privateLog = new ChannelLog(this);
    m_privateLog->setChannel("#private");
    m_privateLog->setMode(ChannelLog::Plain);
  }
  
  QString address = m_settings->getString("ListenAddress");
  quint16 port    = quint16(m_settings->getInt("ListenPort"));
  bool result     = m_server.listen(QHostAddress(address), port);
  
  if (result) {
    LOG(0, tr("IMPOMEZIA Simple Chat Daemon успешно запущен, адрес %1, порт %2").arg(address).arg(port));
    link();
  }
  else {
    LOG(0, tr("Ошибка запуска IMPOMEZIA Simple Chat Daemon, %1").arg(m_server.errorString()));
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
    connect(service, SIGNAL(message(const QString &, const QString &, const QString &)), SLOT(message(const QString &, const QString &, const QString &)));
    connect(service, SIGNAL(newNick(quint8, const QString &, const QString &, const QString &)), SLOT(newNick(quint8, const QString &, const QString &, const QString &)));
    connect(service, SIGNAL(newProfile(quint8, const QString &, const QString &)), SLOT(newProfile(quint8, const QString &, const QString &)));
    connect(service, SIGNAL(newBye(const QString &, const QString &)), SLOT(newBye(const QString &, const QString &)));
    connect(this, SIGNAL(newUser(const QStringList &, bool)), service, SLOT(newUser(const QStringList &, bool)));
    connect(this, SIGNAL(userLeave(const QString &, const QString &, bool)), service, SLOT(userLeave(const QString &, const QString &, bool)));
    connect(this, SIGNAL(message(const QString &, const QString &)), service, SLOT(message(const QString &, const QString &)));
    connect(this, SIGNAL(sendNewNick(quint8, const QString &, const QString &, const QString &)), service, SLOT(sendNewNick(quint8, const QString &, const QString &, const QString &)));
    connect(this, SIGNAL(sendNewProfile(quint8, const QString &, const QString &)), service, SLOT(sendNewProfile(quint8, const QString &, const QString &)));
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
      
      if (m_settings->getBool("ChannelLog"))
        if (list.at(AbstractProfile::Gender) == "male")
          m_channelLog->msg(tr("`%1` зашёл в чат").arg(nick));
        else
          m_channelLog->msg(tr("`%1` зашла в чат").arg(nick));
      
      sendAllUsers(service);
    }
    else
      service->accessDenied(ErrorNickAlreadyUse);
  }  
}


/** [private slots]
 * 
 */
void Daemon::message(const QString &channel, const QString &_sender, const QString &msg)
{
  qDebug() << "Daemon::message()" << channel << _sender << msg;
  
  if (channel.isEmpty()) {
    if (m_settings->getBool("ChannelLog"))
      m_channelLog->msg(tr("%1: %2").arg(_sender).arg(msg));
    
    if (!parseCmd(_sender, msg))
      emit message(_sender, msg);
  }
  else if (m_users.contains(channel)) {
    if (m_settings->getBool("PrivateLog"))
      m_privateLog->msg(tr("`%1` -> `%2`: %3").arg(_sender).arg(channel).arg(msg));
    
    if (!parseCmd(_sender, msg)) {
      DaemonService *senderService = qobject_cast<DaemonService *>(sender());
      if (senderService)
        senderService->privateMessage(1, channel, msg);
      
      DaemonService *service = m_users.value(channel)->service();
      if (service)
        service->privateMessage(0, _sender, msg);
    }
  }
}


/** [private slots]
 * 
 */
void Daemon::newBye(const QString &nick, const QString &bye)
{
  if (m_users.contains(nick)) {
    m_users.value(nick)->profile()->setByeMsg(bye);
  }
}


/** [private slots]
 * 
 */
void Daemon::newNick(quint8 gender, const QString &nick, const QString &newNick, const QString &name)
{
  qDebug() << "Daemon::newNick()";
  
  if (!m_users.contains(nick))
    return;
  
  if (m_users.contains(newNick)) {
    DaemonService *service = qobject_cast<DaemonService *>(sender());
    if (service)
      service->quit();
  }
  else { 
    if (m_settings->getBool("ChannelLog"))
      if (gender)
        m_channelLog->msg(tr("`%1` теперь известна как `%2`").arg(nick).arg(newNick));
      else
        m_channelLog->msg(tr("`%1` теперь известен как `%2`").arg(nick).arg(newNick));
    
    UserUnit *unit = m_users.value(nick);
    m_users.remove(nick);
    m_users.insert(newNick, unit);
    unit->profile()->setGender(gender);
    unit->profile()->setNick(newNick);
    unit->profile()->setFullName(name);
    
    emit sendNewNick(gender, nick, newNick, name);
  }
}


/** [private slots]
 * 
 */
void Daemon::newProfile(quint8 gender, const QString &nick, const QString &name)
{
  qDebug() << "Daemon::newProfile()";
  
  if (!m_users.contains(nick))
    return;
  
  if (m_settings->getBool("ChannelLog"))
    if (gender)
      m_channelLog->msg(tr("`%1` изменила свой профиль").arg(nick));
    else
      m_channelLog->msg(tr("`%1` изменил свой профиль").arg(nick));
  
  UserUnit *unit = m_users.value(nick);
  unit->profile()->setGender(gender);
  unit->profile()->setFullName(name);
  emit sendNewProfile(gender, nick, name);
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
    
    QString bye = unit->profile()->byeMsg();
    if (m_settings->getBool("ChannelLog"))
      if (unit->profile()->isMale())
        m_channelLog->msg(tr("`%1` вышел из чата: %2").arg(nick).arg(bye));
      else
        m_channelLog->msg(tr("`%1` вышла из чата: %2").arg(nick).arg(bye));
    
    delete unit;
    
    emit userLeave(nick, bye, true);
  }
}


/** [private]
 * 
 */
bool Daemon::parseCmd(const QString &nick, const QString &msg)
{
  qDebug() << "Daemon::parseCmd()" << msg;
  
  if (!m_users.contains(nick))
    return false;
  
  // Команда "/server info"
  if (msg.startsWith("/server info", Qt::CaseInsensitive)) {
    DaemonService *service = m_users.value(nick)->service();
    if (service) {
      service->sendServerMessage(serverInfo());
      return true;
    }
    else
      return false;
  }
  else
    return false;
}


/** [private]
 * 
 */
QString Daemon::serverInfo() const
{
  QString info = QString("<b>IMPOMEZIA Simple Chat Daemon %1</b>, ").arg(SCHAT_VERSION);
  
#if   defined(Q_OS_AIX)
  info += "AIX";
#elif defined(Q_OS_BSD4)
  info += "BSD 4.4 ";
#elif defined(Q_OS_BSDI)
  info += "BSD/OS";
#elif defined(Q_OS_CYGWIN)
  info += "Cygwin";
#elif defined(Q_OS_DARWIN)
  info += "Darwin OS";
#elif defined(Q_OS_DGUX)
  info += "DG/UX";
#elif defined(Q_OS_DYNIX)
  info += "DYNIX/ptx";
#elif defined(Q_OS_FREEBSD)
  info += "FreeBSD";
#elif defined(Q_OS_HPUX)
  info += "HP-UX";
#elif defined(Q_OS_HURD)
  info += "GNU Hurd";
#elif defined(Q_OS_IRIX)
  info += "SGI Irix";
#elif defined(Q_OS_LINUX)
  info += "Linux";
#elif defined(Q_OS_LYNX)
  info += "LynxOS";
#elif defined(Q_OS_MSDOS)
  info += "Windows";
#elif defined(Q_OS_NETBSD)
  info += "NetBSD";
#elif defined(Q_OS_OS2)
  info += "OS/2";
#elif defined(Q_OS_OPENBSD)
  info += "OpenBSD";
#elif defined(Q_OS_OS2EMX)
  info += "XFree86 on OS/2 (not PM)";
#elif defined(Q_OS_OSF)
  info += "HP Tru64 UNIX";
#elif defined(Q_OS_QNX6)
  info += "QNX RTP 6.1";
#elif defined(Q_OS_QNX)
  info += "QNX";
#elif defined(Q_OS_RELIANT)
  info += "Reliant UNIX";
#elif defined(Q_OS_SCO)
  info += "SCO OpenServer 5";
#elif defined(Q_OS_SOLARIS)
  info += "Sun Solaris";
#elif defined(Q_OS_ULTRIX)
  info += "DEC Ultrix";
#elif defined(Q_OS_UNIXWARE)
  info += "UnixWare";
#elif defined(Q_OS_WIN32)
  info += "Windows";
#elif defined(Q_OS_WINCE)
  info += "Windows CE";
#endif
  
  info += ", <a href='http://impomezia.net.ru'>http://impomezia.net.ru</a><br />";
  info += tr("Пользователей в сети: <b>%1</b>").arg(m_users.count());
  
  return info;
}


/** [private]
 * 
 */
void Daemon::link()
{
  m_network = new Network(QCoreApplication::instance()->applicationDirPath());
  if (!m_network->fromFile(m_settings->getString("NetworkFile"))) {
    delete m_network;
    m_network = 0;
  }
  else {
    qDebug() << "VALID NETWORK FILE";
  }
}


/** [private]
 * Отправка подключившемуся клиенту списка всех пользователей.
 */
void Daemon::sendAllUsers(DaemonService *service)
{
  if (service) {
    QHashIterator<QString, UserUnit *> i(m_users);
    while (i.hasNext()) {
      i.next();
      
      if (i.value()->service())
        service->newUser(i.value()->profile()->pack(), false);
    }
  }
}
