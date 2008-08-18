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

/*!
 * \class Daemon
 * \brief Сервер чата
 * 
 * Класс полностью включает в себя функциональность сервера чата.
 */

/*!
 * \brief Конструктор класса Daemon.
 * \param parent Указатель на родительский объект.
 */
Daemon::Daemon(QObject *parent)
  : QObject(parent)
{
  m_settings = new DaemonSettings(this);
  m_remoteNumeric = 0;
  connect(&m_server, SIGNAL(newConnection()), SLOT(incomingConnection()));
}


/*!
 * \brief Запуск сервера.
 * 
 * Функция читает настройки сервера, устанавливает параметры логирования каналов
 * и производит запуск сервера \a m_server.
 * Данные о попытке запуска заносятся в лог файл.
 * \return \a true в случае успешного запуска, и \a false при возникновении любой ошибки.
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


/*!
 * \brief Обслуживание нового входящего соединения.
 * 
 * При наличии ожидающего соединения создаётся класс DaemonService получающий указатель на \a QTcpSocket.
 * Функция также создаёт базовые соединения сигнал/слот, общие для клиентского и межсерверного соединения.
 */
void Daemon::incomingConnection()
{
  if (m_server.hasPendingConnections()) {
    DaemonService *service = new DaemonService(m_server.nextPendingConnection(), this);
    connect(service, SIGNAL(greeting(const QStringList &, quint8)), SLOT(greeting(const QStringList &, quint8)));
    connect(service, SIGNAL(leave(const QString &, quint8)), SLOT(serviceLeave(const QString &, quint8)));
    connect(this, SIGNAL(userLeave(const QString &, const QString &, quint8)), service, SLOT(sendUserLeave(const QString &, const QString &, quint8)));
    connect(this, SIGNAL(newUser(const QStringList &, quint8, quint8)), service, SLOT(sendNewUser(const QStringList &, quint8, quint8)));
    connect(this, SIGNAL(sendNewLink(quint8, const QString &, const QString &)), service, SLOT(sendNewLink(quint8, const QString &, const QString &)));
    connect(this, SIGNAL(sendLinkLeave(quint8, const QString &, const QString &)), service, SLOT(sendLinkLeave(quint8, const QString &, const QString &)));
  }
}


/*!
 * \brief Уведомление об успешном установлении связи с вышестоящим сервером.
 * 
 * Функция устанавливает номер вышестоящего сервера и добавляет его номер в список номеров.
 * \param network Параметр игнорируется и не используется.
 * \param server Параметр игнорируется и не используется.
 * \param numeric Номер вышестоящего сервера.
 */
void Daemon::clientAccessGranted(const QString &/*network*/, const QString &/*server*/, quint16 numeric)
{
  m_remoteNumeric = numeric;
  m_numerics << numeric;
}


/*!
 * \brief Обработка потери соединения с вышестоящим сервером.
 * 
 * Функция сбрасывает \a m_remoteNumeric, удаляет не существующие больше номера серверов из \a m_numerics
 * и пользователей, которые были к ним подключены из \a m_users с рассылкой сигнала об отключении пользователя.
 * \param echo Параметр игнорируется и не используется.
 */
void Daemon::clientServiceLeave(bool /*echo*/)
{
  m_remoteNumeric = 0;
  m_numerics.clear();
  m_numerics << m_numeric;
  m_numerics += m_links.keys();

  QHashIterator<QString, UserUnit *> i(m_users);
  while (i.hasNext()) {
    i.next();
    if (!m_numerics.contains(i.value()->numeric()))
      userLeave(i.value()->profile()->nick());
  }
}


/*!
 * \brief Синхронизация события подключения к удалённому серверу нового пользователя.
 * 
 * Слот вызывается при получении пакета \b OpcodeNewUser от локального клиентского подключения либо от удалённого сервера.
 * Функция вносит нового пользователя в список \a m_users и высылает уведомление.
 * \param list Стандартный список, содержащий в себе полные данные пользователя.
 * \param echo Параметр игнорируется и не используется.
 * \param numeric Номер сервера, к которому подключен данный пользователь.
 */
void Daemon::clientSyncUsers(const QStringList &list, quint8 /*echo*/, quint8 numeric)
{ /// \todo сделать проверку на коллизию ников.
  QString nick = list.at(AbstractProfile::Nick);

  if (!m_users.contains(nick)) {
    m_users.insert(nick, new UserUnit(list, 0, numeric));
    emit newUser(list, 1, numeric);
  }
}


/*!
 * \brief Уведомление о завершении передачи списка пользователей от вышестоящего сервера.
 * 
 * Функция высылает в ответ список локальных пользователей.
 */
void Daemon::clientSyncUsersEnd()
{
  if (m_remoteNumeric) {
    QHashIterator<QString, UserUnit *> i(m_users);
    while (i.hasNext()) {
      i.next();
      quint8 numeric = i.value()->numeric();
      if (numeric == m_numeric || m_links.contains(numeric))
        m_link->sendNewUser(i.value()->profile()->pack(), 0, numeric);
    }
  }
}


/*!
 * \brief Синхронизация события отключения пользователя от удалённого сервера.
 * 
 * Функция устанавливает в профиле пользователя новое сообщение о выходе и вызывает функцию userLeave(const QString &nick).
 * \param nick Ник пользователя.
 * \param bye Сообщение о выходе.
 * \param flag Параметр игнорируется и не используется.
 * \sa userLeave(const QString &nick)
 */
void Daemon::clientUserLeave(const QString &nick, const QString &bye, quint8 /*flag*/)
{
  if (m_users.contains(nick))
    m_users.value(nick)->profile()->setByeMsg(bye);

  userLeave(nick);
}


/*!
 * \brief Обработка приветствия от подключенного клиента.
 * 
 * В зависимости от значения \a flag вызывается функция greetingLink(const QStringList &list, DaemonService *service) для сервера
 * либо greetingUser(const QStringList &list, DaemonService *service) для клиента.
 * \param list Стандартный список, содержащий в себе полные данные пользователя.
 * \param flag Флаг подключения.
 * \sa greetingLink(const QStringList &list, DaemonService *service), greetingUser(const QStringList &list, DaemonService *service)
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


/*!
 * \brief Уведомление об отключении от удалённого сервера другого сервера.
 * 
 * Номер сервера удаляется из списка серверов и высылается соответствующие уведомление. 
 * \param numeric Номер сервера подключившегося к сети.
 * \param network Название сети.
 * \param ip Адрес сервера
 * \sa newLink(quint8 numeric, const QString &network, const QString &ip)
 */
void Daemon::linkLeave(quint8 numeric, const QString &network, const QString &ip)
{
  m_numerics.removeAll(numeric);

  emit sendLinkLeave(numeric, network, ip);
}


/*!
 * \brief Обработка нового сообщения от локального пользователя.
 * 
 * Для всех сообщений проверяется наличие в них команды для сервера.
 * 
 * Если \a channel пустая строка, то это сообщение предназначено для отправки в главный канал.
 * Происходит рассылка уведомление для локальных клиентов и остальных серверов, при необходимости добавляется запись в канальный лог.
 * 
 * Отправка приватных сообщений производится различными способами в зависимости от того к какому серверу подключен получатель.
 * Получатель может быть:
 *  - Подключенным локально к этому серверу (numeric == m_numeric).
 *  - Подключенным к одному из серверов подключенных к данному серверу (m_links.contains(numeric)).
 *  - Подключенным к серверу к которому невозможно обратится на прямую, в этом случае при наличии клиентского подключения,
 * сообщение передаётся на вышестоящий сервер.
 * 
 * \param channel Канал/ник для кого предназначено сообщение (пустая строка - главный канал).
 * \param nick Ник отправителя сообщения.
 * \param msg Сообщение.
 * \sa parseCmd(const QString &nick, const QString &msg).
 */
void Daemon::message(const QString &channel, const QString &nick, const QString &msg)
{
#ifdef SCHAT_DEBUG
  qDebug() << "Daemon::message(const QString &, const QString &, const QString &)" << channel << nick << msg;
#endif

  if (channel.isEmpty()) {
    if (m_settings->getBool("ChannelLog"))
      m_channelLog->msg(tr("%1: %2").arg(nick).arg(msg));

    if (!parseCmd(nick, msg)) {
      emit sendMessage(nick, msg);
      if (m_network) {
        emit sendRelayMessage(channel, nick, msg, m_numeric);
        if (m_remoteNumeric)
          m_link->sendRelayMessage(channel, nick, msg, m_numeric);
      }
    }
  }
  else if (m_users.contains(channel)) {
    if (m_settings->getBool("PrivateLog"))
      m_privateLog->msg(tr("`%1` -> `%2`: %3").arg(nick).arg(channel).arg(msg));

    if (!parseCmd(nick, msg)) {
      quint16 numeric = m_users.value(channel)->numeric();
      DaemonService *senderService = qobject_cast<DaemonService *>(sender());
      if (!senderService)
        return;
      bool err = true;
      
      if (numeric == m_numeric) {
        err = false;
        DaemonService *service = m_users.value(channel)->service();
        if (service)
          service->sendPrivateMessage(0, nick, msg);
      }
      else if (m_links.contains(numeric)) {
        err = false;        
        DaemonService *service = m_links.value(numeric)->service();
        if (service)
          service->sendRelayMessage(channel, nick, msg, m_numeric);
      }
      else if (m_remoteNumeric) {
        err = false;
        m_link->sendRelayMessage(channel, nick, msg, m_numeric);
      }
      
      if (!err)
        senderService->sendPrivateMessage(1, channel, msg);
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


/*!
 * \brief Уведомление о подключении к удалённому серверу другого сервера.
 * 
 * Номер нового сервера добавляется в список серверов и высылается уведомление о новом сервере. 
 * \param numeric Номер сервера подключившегося к сети.
 * \param network Название сети.
 * \param ip Адрес сервера
 * \sa linkLeave(quint8 numeric, const QString &network, const QString &ip)
 */
void Daemon::newLink(quint8 numeric, const QString &network, const QString &ip)
{
  if (!m_numerics.contains(numeric))
    m_numerics << numeric;

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


/*!
 * \brief Обработка сообщения пользователя полученного с другого сервера.
 * 
 * Если \a channel пустая строка, то это сообщение предназначено для отправки в главный канал. Происходит рассылка уведомление для локальных клиентов и остальных серверов, при необходимости добавляется запись в канальный лог.
 * 
 * Отправка приватных сообщений произовдится различными способами в зависимости от того к какому серверу подключен получатель. Получатель может быть:
 *  - Подключенным локально к этому серверу (numeric == m_numeric).
 *  - Подключенным к одному из серверов подкюченных к данному серверу (m_links.contains(numeric)).
 * 
 * \param channel Канал/ник для кого предназначено сообщение (пустая строка - главный канал).
 * \param sender  Ник отправителся.
 * \param msg Cообщение.
 * \param numeric Номер сервера на котором находится пользователь отправивший сообщение.
 */
void Daemon::relayMessage(const QString &channel, const QString &sender, const QString &msg, quint8 /*numeric*/)
{
#ifdef SCHAT_DEBUG
  qDebug() << "Daemon::relayMessage()" << channel << sender << msg;
#endif

  if (!m_network)
    return;

  if (channel.isEmpty()) {
    if (m_settings->getBool("ChannelLog"))
      m_channelLog->msg(tr("%1: %2").arg(sender).arg(msg));

    emit sendMessage(sender, msg);
    emit sendRelayMessage("", sender, msg, m_numeric);
    return;
  }

  quint8 numeric = m_users.value(channel)->numeric();
  
  if (m_settings->getBool("PrivateLog"))
    m_privateLog->msg(tr("`%1` -> `%2`: %3").arg(sender).arg(channel).arg(msg));

  if (numeric == m_numeric) {
    DaemonService *service = m_users.value(channel)->service();
    if (service)
      service->sendPrivateMessage(0, sender, msg);
  }
  else if (m_links.contains(numeric)) {
    DaemonService *service = m_links.value(numeric)->service();
    if (service)
      service->sendRelayMessage(channel, sender, msg, numeric);
  }
}


/*!
 * \brief Обработка отключения авторизированного клиента.
 * 
 * В зависимости от значения flag вызывается функция linkLeave(const QString &nick) для сервера либо userLeave(const QString &nick) для клиента.
 * \param nick Ник пользователя.
 * \param flag Флаг подключения.
 * \sa linkLeave(const QString &nick), userLeave(const QString &nick)
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


/*!
 * \brief Обработка команд предназначенных для сервера.
 * 
 * \param nick Ник пользователя отправившего сообщение.
 * \param msg Сообщение.
 * \return \a true если команда опознана и выполнена, \a false при возникновении любой ошибки.
 * \todo Доработать команду /server info и добавить команду /servers
 */
bool Daemon::parseCmd(const QString &nick, const QString &msg)
{
  if (!m_users.contains(nick))
    return false;

  /// Команда "/server info"
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


/*!
 * \brief Обработка приветствия от удалённого сервера.
 * 
 * Для успешного подключения должны быть соблюдены следующие условия:
 *  - Сконфигурированная сеть \a m_network.
 *  - Должны совпадать ключи сети.
 *  - Удалённый сервер должен иметь уникальный номер.
 * 
 * При успешном подключении сервис уведомляется об этом, также производится синхронизация номеров сети и пользователей.
 * Функция устанавливает соединения сигнал/слот специфичные для данного типа соединения.
 * 
 * Факт подключения записывается в журнал.
 * 
 * \param list Стандартный список, содержащий в себе полные данные пользователя.
 * \param service Указатель на сервис.
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
        connect(service, SIGNAL(newUser(const QStringList &, quint8, quint8)), SLOT(clientSyncUsers(const QStringList &, quint8, quint8)));
        connect(service, SIGNAL(userLeave(const QString &, const QString &, quint8)), SLOT(clientUserLeave(const QString &, const QString &, quint8)));
        connect(this, SIGNAL(sendRelayMessage(const QString &, const QString &, const QString &, quint8)), service, SLOT(sendRelayMessage(const QString &, const QString &, const QString &, quint8)));
        service->accessGranted(m_numeric);
        service->sendNumerics(m_numerics);

        emit sendNewLink(numeric, m_network->name(), list.at(AbstractProfile::Host));
        
        sendAllUsers(service);

        LOG(0, tr("- Notice - Connect Link: %1@%2, %3").arg(numeric).arg(list.at(AbstractProfile::Host)).arg(list.at(AbstractProfile::UserAgent)));
        /// \todo Необходимо добавить запись в канальный лог
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


/*!
 * \brief Обработка приветствия от клиента.
 * 
 * \param list Стандартный список, содержащий в себе полные данные пользователя.
 * \param service Указатель на сервис.
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
    connect(this, SIGNAL(sendNewNick(quint8, const QString &, const QString &, const QString &)), service, SLOT(sendNewNick(quint8, const QString &, const QString &, const QString &)));
    connect(this, SIGNAL(sendNewProfile(quint8, const QString &, const QString &)), service, SLOT(sendNewProfile(quint8, const QString &, const QString &)));
    connect(this, SIGNAL(sendMessage(const QString &, const QString &)), service, SLOT(sendMessage(const QString &, const QString &)));
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
    if (m_network && m_remoteNumeric)
      m_link->sendNewUser(list, 1, m_numeric);
  }
  else
    service->accessDenied(ErrorNickAlreadyUse);
}


/** [private]
 * Инициализирует файл сети "NetworkFile".
 * Если "Numeric" этого сервера равен `0`, то выходим из функции, без создания соединения.
 * В случае успешной инициализации сети, создаём локальный профиль `m_profile` и
 * если указан адрес вышестоящего сервера, то производится попытка подключения.
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
      connect(m_link, SIGNAL(newUser(const QStringList &, quint8, quint8)), SLOT(clientSyncUsers(const QStringList &, quint8, quint8)));
      connect(m_link, SIGNAL(accessGranted(const QString &, const QString &, quint16)), SLOT(clientAccessGranted(const QString &, const QString &, quint16)));
      connect(m_link, SIGNAL(syncUsersEnd()), SLOT(clientSyncUsersEnd()));
      connect(m_link, SIGNAL(unconnected(bool)), SLOT(clientServiceLeave(bool)));
      connect(m_link, SIGNAL(userLeave(const QString &, const QString &, quint8)), SLOT(clientUserLeave(const QString &, const QString &, quint8)));      
      m_link->connectToHost();
    }
  }
}


/*!
 * \brief Обработка отключения нижестоящего сервера.
 * 
 * Сервер удаляется из списка подключенных серверов и его номер удаляется из списка номеров серверов.
 * Высылается уведомление об отключении сервера, и отключаются все клиенты, ассоциированные с этим сервером.
 * Событие записывается в журнал.
 * \param nick Номер сервера в виде строки.
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

      QHashIterator<QString, UserUnit *> i(m_users);
      while (i.hasNext()) {
        i.next();
        if (i.value()->numeric() == numeric)
          userLeave(i.value()->profile()->nick());
      }

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
      service->sendNewUser(i.value()->profile()->pack(), 0, i.value()->numeric());
    }
    service->sendSyncUsersEnd();
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

    emit userLeave(nick, bye, 1);

    if (m_network && m_remoteNumeric)
      m_link->sendUserLeave(nick, bye, 1);
  }
}
