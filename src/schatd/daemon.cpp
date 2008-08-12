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
#include "linkunit.h"
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
  m_remoteNumeric = 0;
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
    LOG(0, tr("- Notice - `IMPOMEZIA Simple Chat Daemon` успешно запущен, адрес %1, порт %2").arg(address).arg(port));
    link();
  }
  else {
    LOG(0, tr("- Error - Ошибка запуска `IMPOMEZIA Simple Chat Daemon`, [%1]").arg(m_server.errorString()));
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
    connect(service, SIGNAL(greeting(const QStringList &, quint8)), SLOT(greeting(const QStringList &, quint8)));
    connect(service, SIGNAL(leave(const QString &, quint8)), SLOT(serviceLeave(const QString &, quint8)));
    connect(this, SIGNAL(sendNewLink(quint8, const QString &, const QString &)), service, SLOT(sendNewLink(quint8, const QString &, const QString &)));
    connect(this, SIGNAL(sendLinkLeave(quint8, const QString &, const QString &)), service, SLOT(sendLinkLeave(quint8, const QString &, const QString &)));
  }
}


/** [private slots]
 * Слот вызывается сигналом `greeting(const QStringList &)` от сервиса ожидающего
 * проверки приветствия (проверка на дубдикат ников).
 * В случае успеха сервис уведомляется (`accessGranted()`) об этом, добавляется в список пользователей
 * и высылается сигнал `newUser(const QStringList &list)`.
 * В случае дубликата ников высылается код ошибки `ErrorNickAlreadyUse`.
 */
void Daemon::greeting(const QStringList &list, quint8 flag)
{
#ifdef SCHAT_DEBUG
  qDebug() << "Daemon::greeting(const QStringList &)" << list.at(AbstractProfile::Nick) << flag;
#endif

  DaemonService *service = qobject_cast<DaemonService *>(sender());
  if (service)  
    if (flag == FlagLink)
      greetingLink(list, service);
    else
      greetingUser(list, service);

}


/** [private slots]
 * 
 */
void Daemon::linkAccessGranted(const QString &/*network*/, const QString &/*server*/, quint16 numeric)
{
  m_remoteNumeric = numeric;
  m_numerics << numeric;
}


/** [private slots]
 * 
 */
void Daemon::linkLeave(quint8 numeric, const QString &network, const QString &ip)
{
#ifdef SCHAT_DEBUG
  qDebug() << "Daemon::linkLeave()" << numeric << network << ip;
#endif
  emit sendLinkLeave(numeric, network, ip);
}


/** [private slots]
 * Ретрансляция пакета `OpcodeMessage`, полученного через клиентское подключение `m_link`.
 */
void Daemon::linkMessage(const QString &sender, const QString &msg)
{
#ifdef SCHAT_DEBUG
  qDebug() << "Daemon::linkMessage()" << sender << msg;
#endif
  if (m_settings->getBool("ChannelLog"))
    m_channelLog->msg(tr("%1: %2").arg(sender).arg(msg));

  emit message(sender, msg);
}


/** [private slots]
 * 
 */
void Daemon::message(const QString &channel, const QString &_sender, const QString &msg)
{
#ifdef SCHAT_DEBUG
  qDebug() << "Daemon::message(const QString &, const QString &, const QString &)" << channel << _sender << msg;
#endif
  
  if (channel.isEmpty()) {
    if (m_settings->getBool("ChannelLog"))
      m_channelLog->msg(tr("%1: %2").arg(_sender).arg(msg));
    
    if (!parseCmd(_sender, msg)) {
      emit message(_sender, msg);
      if (m_network) {
        emit sendRelayMessage("", _sender, msg, m_numeric);
        if (m_network->count() > 0)
          m_link->sendRelayMessage(channel, _sender, msg, m_numeric);
      }
    }
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
  if (m_users.contains(nick))
    m_users.value(nick)->profile()->setByeMsg(bye);
}


/** [private slots]
 * 
 */
void Daemon::newLink(quint8 numeric, const QString &network, const QString &ip)
{
#ifdef SCHAT_DEBUG
  qDebug() << "Daemon::newLink()" << numeric << network << ip;
#endif
  emit sendNewLink(numeric, network, ip);
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
 * Слот вызывается при получении пакета `OpcodeRelayMessage` в клиентском сервисе `m_link` (вышестоящий сервер)
 * либо от одного из линков (нижестоящий сервер).
 *  ---
 * const QString &channel -> канал/ник для кого предназначено сообщение (пустая строка - главный канал).
 * const QString &sender  -> ник отправителся.
 * const QString &msg     -> сообщение.
 * quint8 numeric         -> numeric сервера на котором находится пользователь отправивший сообщение.
 */
void Daemon::relayMessage(const QString &channel, const QString &sender, const QString &msg, quint8 numeric)
{
#ifdef SCHAT_DEBUG
  qDebug() << "Daemon::relayMessage()" << channel << sender << msg << numeric;
#endif

  if (!m_network)
    return;

  if (channel.isEmpty()) {
    if (m_settings->getBool("ChannelLog"))
      m_channelLog->msg(tr("%1: %2").arg(sender).arg(msg));

    emit message(sender, msg);
    emit sendRelayMessage("", sender, msg, numeric);
//    if (m_network)
//      if (m_network->count() > 0 && m_links.contains(numeric))
//        m_link->sendRelayMessage("", sender, msg, numeric);
  }
}

/** [private slots]
 * Слот вызывается сигналом `leave(const QString &)` из сервиса получившего авторизацию
 * перед удалением.
 * Пользователь удаляется из списка пользователей и объект `UserUnit` уничтожается.
 */
void Daemon::serviceLeave(const QString &nick, quint8 flag)
{
#ifdef SCHAT_DEBUG
  qDebug() << "Daemon::serviceLeave(const QString &)" << nick << flag;
#endif

  if (flag == FlagLink)
    linkLeave(nick);
  else
    userLeave(nick);
}


/** [private slots]
 * 
 */
void Daemon::syncNumerics(const QList<quint8> &numerics)
{
  qDebug() << "Daemon::syncNumerics()" << numerics;
  foreach (quint8 numeric, numerics) {
    if (!m_numerics.contains(numeric))
      m_numerics << numeric;
  }
}


/** [private slots]
 * Слот вызывается когда клиентский сокет получает пакет `OpcodeNewUser`.
 */
void Daemon::syncUsers(const QStringList &list, quint8 echo, quint8 numeric)
{
  QString nick = list.at(AbstractProfile::Nick);

  if (!m_users.contains(nick)) {
    m_users.insert(nick, new UserUnit(list, 0, numeric));
    emit newUser(list, echo, numeric);
  }
}


/** [private]
 * 
 */
bool Daemon::parseCmd(const QString &nick, const QString &msg)
{
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
void Daemon::greetingLink(const QStringList &list, DaemonService *service)
{
  quint16 err = 0;
  quint8 numeric = quint8(QString(list.at(AbstractProfile::Nick)).toInt());
  
  if (m_network) {
    if (m_network->key() == list.at(AbstractProfile::FullName)) {

      if (!m_numerics.contains(numeric)) {
        m_links.insert(numeric, new LinkUnit(list.at(AbstractProfile::Host), service));
        m_numerics << numeric;
        connect(service, SIGNAL(relayMessage(const QString &, const QString &, const QString &, quint8)), SLOT(relayMessage(const QString &, const QString &, const QString &, quint8)));
        connect(this, SIGNAL(sendRelayMessage(const QString &, const QString &, const QString &, quint8)), service, SLOT(sendRelayMessage(const QString &, const QString &, const QString &, quint8)));
        service->accessGranted(m_numeric);
        service->sendNumerics(m_numerics);

        emit sendNewLink(numeric, m_network->name(), list.at(AbstractProfile::Host));
        
        sendAllUsers(service);

        LOG(0, tr("- Notice - Connect Link: %1@%2, %3").arg(numeric).arg(list.at(AbstractProfile::Host)).arg(list.at(AbstractProfile::UserAgent)));
        // TODO добавить запись в канальный лог
      }
      else
        err = ErrorNumericAlreadyUse;
    }
    else
      err = ErrorBadNetworkKey;
  }
  else
    err = ErrorNotNetworkConfigured;

  if (err) {
    LOG(0, tr("- Warning - Отказ в доступе серверу: %1@%2, код ошибки: %3").arg(numeric).arg(list.at(AbstractProfile::Host)).arg(err));
    service->accessDenied(err);
  }
}


/** [private]
 * 
 */
void Daemon::greetingUser(const QStringList &list, DaemonService *service)
{
  QString nick = list.at(AbstractProfile::Nick);
  
  if (!m_users.contains(nick)) {
    m_users.insert(nick, new UserUnit(list, service, m_numeric));
    connect(service, SIGNAL(newNick(quint8, const QString &, const QString &, const QString &)), SLOT(newNick(quint8, const QString &, const QString &, const QString &)));
    connect(service, SIGNAL(newProfile(quint8, const QString &, const QString &)), SLOT(newProfile(quint8, const QString &, const QString &)));
    connect(service, SIGNAL(newBye(const QString &, const QString &)), SLOT(newBye(const QString &, const QString &)));
    connect(service, SIGNAL(message(const QString &, const QString &, const QString &)), SLOT(message(const QString &, const QString &, const QString &)));
    connect(this, SIGNAL(newUser(const QStringList &, quint8, quint8)), service, SLOT(newUser(const QStringList &, quint8, quint8)));
    connect(this, SIGNAL(userLeave(const QString &, const QString &, bool)), service, SLOT(userLeave(const QString &, const QString &, bool)));
    connect(this, SIGNAL(sendNewNick(quint8, const QString &, const QString &, const QString &)), service, SLOT(sendNewNick(quint8, const QString &, const QString &, const QString &)));
    connect(this, SIGNAL(sendNewProfile(quint8, const QString &, const QString &)), service, SLOT(sendNewProfile(quint8, const QString &, const QString &)));
    connect(this, SIGNAL(message(const QString &, const QString &)), service, SLOT(message(const QString &, const QString &)));
    service->accessGranted(m_numeric);
    emit newUser(list, 1, m_numeric);
  
    LOG(0, tr("- Notice - Connect: %1@%2, %3, %4, %5")
        .arg(list.at(AbstractProfile::Nick))
        .arg(list.at(AbstractProfile::Host))
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


/** [private]
 * Инициализирует файл сети "NetworkFile".
 * В случае успеха устанавливает numeric сервера "Numeric", создаёт профиль `m_profile`
 * И если указан адрес вышестоящего сервера, то производится попытка подключения.
 * Если инициализация сети прошла с ошибкой `m_network` устанавливается в `0`.
 */
void Daemon::link()
{
  m_numeric = quint8(m_settings->getInt("Numeric"));
  if (!m_numeric) {
    m_network = 0;
    return;
  }

  m_network = new Network(QCoreApplication::instance()->applicationDirPath());
  if (!m_network->fromFile(m_settings->getString("NetworkFile"))) {
    delete m_network;
    m_network = 0;
  }
  else {
    m_profile = new AbstractProfile(this);
    m_profile->setNick(QString().number(m_settings->getInt("Numeric")));
    m_profile->setFullName(m_network->key());
    m_numerics << m_numeric;
        
    if (m_network->count() > 0) {
      m_link = new ClientService(m_profile, m_network, this);
      connect(m_link, SIGNAL(newLink(quint8, const QString &, const QString &)), SLOT(newLink(quint8, const QString &, const QString &)));
      connect(m_link, SIGNAL(linkLeave(quint8, const QString &, const QString &)), SLOT(linkLeave(quint8, const QString &, const QString &)));
      connect(m_link, SIGNAL(relayMessage(const QString &, const QString &, const QString &, quint8)), SLOT(relayMessage(const QString &, const QString &, const QString &, quint8)));
      connect(m_link, SIGNAL(syncNumerics(const QList<quint8> &)), SLOT(syncNumerics(const QList<quint8> &)));
      connect(m_link, SIGNAL(newUser(const QStringList &, quint8, quint8)), SLOT(syncUsers(const QStringList &, quint8, quint8)));
      connect(m_link, SIGNAL(accessGranted(const QString &, const QString &, quint16)), SLOT(linkAccessGranted(const QString &, const QString &, quint16)));
      m_link->connectToHost();
    }
  }
}


/** [private]
 * 
 */
void Daemon::linkLeave(const QString &nick)
{
  if (m_network) {
    quint8 numeric = quint8(nick.toInt());

    if (m_links.contains(numeric)) {
      LinkUnit *unit = m_links.value(numeric);
      m_links.remove(numeric);
      m_numerics.removeAll(numeric);

      LOG(0, tr("- Notice - Disconnect Link: %1@%2").arg(nick).arg(unit->host()));

      emit sendLinkLeave(numeric, m_network->name(), unit->host());
      delete unit;
    }
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
      service->newUser(i.value()->profile()->pack(), 0, i.value()->numeric());
    }
  }
}


/** [private]
 * 
 */
void Daemon::userLeave(const QString &nick)
{
  if (m_users.contains(nick)) {
    UserUnit *unit = m_users.value(nick);
    m_users.remove(nick);

    LOG(0, tr("- Notice - Disconnect: %1@%2").arg(nick).arg(unit->profile()->host()));

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

