/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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

#ifndef DISABLE_LOCAL_SERVER
  #include "ipc/localservice.h"
#endif

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
  m_settings = new DaemonSettings(QCoreApplication::instance()->applicationDirPath() + "/schatd.conf", this);
  m_remoteNumeric = 0;
  m_syncUsers = false;
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

  m_maxLinks = m_settings->getInt("MaxLinks");
  m_maxUsers = m_settings->getInt("MaxUsers");
  m_maxUsersPerIp = m_settings->getInt("MaxUsersPerIp");

  #ifndef DISABLE_LOCAL_SERVER
    if (m_settings->getBool("LocalServer")) {
      m_localServer = new QLocalServer(this);
      if (m_localServer->listen(QCryptographicHash::hash(qApp->applicationDirPath().toUtf8(), QCryptographicHash::Md5).toHex())) {
        connect(m_localServer, SIGNAL(newConnection()), SLOT(incomingLocalConnection()));
      }
      else
        m_localServer->deleteLater();
    }
  #endif /*DISABLE_LOCAL_SERVER*/

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

  emit sendLinkLeave(m_numeric, m_network->name(), m_profile->byeMsg());

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
{
  QString nick = list.at(AbstractProfile::Nick);

  if (m_users.contains(nick) && !m_syncUsers) {
    DaemonService *service = m_users.value(nick)->service();
    if (service)
      service->quit(true);
    userLeave(nick);
  }

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
  m_syncUsers = true;
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
        emit sendRelayMessage(channel, nick, msg);
        if (m_remoteNumeric)
          m_link->sendRelayMessage(channel, nick, msg);
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
          service->sendRelayMessage(channel, nick, msg);
      }
      else if (m_remoteNumeric) {
        err = false;
        m_link->sendRelayMessage(channel, nick, msg);
      }

      if (!err)
        senderService->sendPrivateMessage(1, channel, msg);
    }
  }
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
 */
void Daemon::relayMessage(const QString &channel, const QString &sender, const QString &msg)
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

    if (m_users.contains(sender)) {
      quint8 senderNum = m_users.value(sender)->numeric();

      QHashIterator<quint8, LinkUnit *> i(m_links);
      while (i.hasNext()) {
        i.next();
        if (i.key() != senderNum && i.value()->service())
          i.value()->service()->sendRelayMessage("", sender, msg);
      }
    }
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
      service->sendRelayMessage(channel, sender, msg);
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
  foreach (quint8 numeric, numerics) {
    if (!m_numerics.contains(numeric))
      m_numerics << numeric;
  }
}


#ifndef DISABLE_LOCAL_SERVER
void Daemon::incomingLocalConnection()
{
  if (m_localServer->hasPendingConnections()) {
    new LocalService(m_localServer->nextPendingConnection(), this);
  }
}
#endif /*DISABLE_LOCAL_SERVER*/


/*!
 * \brief Обработка команд предназначенных для сервера.
 *
 * \param nick Ник пользователя отправившего сообщение.
 * \param msg Сообщение.
 * \return \a true если команда опознана и выполнена, \a false при возникновении любой ошибки.
 */
bool Daemon::parseCmd(const QString &nick, const QString &msg)
{
  if (!m_users.contains(nick))
    return false;

  QString text = ChannelLog::toPlainText(msg).trimmed().toLower();

  /// Команда "/server"
  if (text == "/server") {
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


int Daemon::localLinksCount() const
{
  int out = 0;
  QHashIterator<quint8, LinkUnit *> i(m_links);
  while (i.hasNext()) {
    i.next();
    if (i.value()->service())
      ++out;
  }

  return out;
}


int Daemon::localUsersCount() const
{
  int out = 0;
  QHashIterator<QString, UserUnit *> i(m_users);
  while (i.hasNext()) {
    i.next();
    if (i.value()->numeric() == m_numeric)
      ++out;
  }

  return out;
}


/*!
 * \brief Возвращает html форматированную строку содержащую информацию о сервере.
 *
 * \return Строка с информацией о сервере.
 */
QString Daemon::serverInfo() const
{
  QString info = QString("<b>IMPOMEZIA Simple Chat Daemon %1</b>, <a href='http://impomezia.com'>http://impomezia.com</a>"
      "<table><tr><td class='info'>Платформа: </td><td><b>").arg(SCHAT_VERSION);

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

  info += tr(", Qt %1</b></td></tr>").arg(qVersion());

  if (m_network) {
    info += tr("<tr><td class='info'>Название сети: </td><td><b>%1</b></td></tr>").arg(m_network->name());
    info += tr("<tr><td class='info'>Пользователей: </td><td><b>%1</b></td></tr>").arg(m_users.count());
    info += tr("<tr><td class='info'>Серверов: </td><td><b>%1</b></td></tr></table>").arg(m_numerics.count());
  }
  else
    info += tr("<tr><td class='info'>Пользователей: </td><td><b>%1</b></td></tr></table>").arg(m_users.count());

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

        if (m_maxLinks > 0) {
          if (m_maxLinks == localLinksCount()) {
            service->accessDenied(ErrorLinksLimitExceeded);
            return;
          }
        }

        m_links.insert(numeric, new LinkUnit(list.at(AbstractProfile::ByeMsg), service));
        m_numerics << numeric;
        connect(service, SIGNAL(newNick(quint8, const QString &, const QString &, const QString &)), SLOT(syncProfile(quint8, const QString &, const QString &, const QString &)));
        connect(service, SIGNAL(relayMessage(const QString &, const QString &, const QString &)), SLOT(relayMessage(const QString &, const QString &, const QString &)));
        connect(service, SIGNAL(newUser(const QStringList &, quint8, quint8)), SLOT(clientSyncUsers(const QStringList &, quint8, quint8)));
        connect(service, SIGNAL(userLeave(const QString &, const QString &, quint8)), SLOT(clientUserLeave(const QString &, const QString &, quint8)));
        connect(service, SIGNAL(newBye(const QString &, const QString &)), SLOT(syncBye(const QString &, const QString &)));
        connect(this, SIGNAL(sendRelayMessage(const QString &, const QString &, const QString &)), service, SLOT(sendRelayMessage(const QString &, const QString &, const QString &)));
        connect(this, SIGNAL(sendSyncProfile(quint8, const QString &, const QString &, const QString &)), service, SLOT(sendNewNick(quint8, const QString &, const QString &, const QString &)));
        connect(this, SIGNAL(sendSyncBye(const QString &, const QString &)), service, SLOT(sendSyncBye(const QString &, const QString &)));
        service->accessGranted(m_numeric);
        service->sendNumerics(m_numerics);

        emit sendNewLink(numeric, m_network->name(), list.at(AbstractProfile::ByeMsg));

        sendAllUsers(service);

        LOG(0, tr("- Notice - Connect Link: %1@%2, %3").arg(numeric).arg(list.at(AbstractProfile::Host)).arg(list.at(AbstractProfile::UserAgent)));
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

    if (m_maxUsers > 0) {
      if (m_maxUsers == localUsersCount()) {
        service->accessDenied(ErrorUsersLimitExceeded);
        return;
      }
    }

    if (m_maxUsersPerIp > 0) {
      QString ip = list.at(AbstractProfile::Host);
      if (m_ipLimits.contains(ip)) {
        int hosts = m_ipLimits.value(ip);
        if (hosts == m_maxUsersPerIp) {
          service->accessDenied(ErrorMaxUsersPerIpExceeded);
          return;
        }
        else {
          m_ipLimits.insert(ip, hosts + 1);
        }
      }
      else {
        m_ipLimits.insert(ip, 1);
      }
    }

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


/*!
 * \brief Инициализирует поддержку сети и при необходимости устанавливает соединение с вышестоящим сервером.
 *
 * Для успешной инициализации сети необходимы следубщие условия
 * - "Numeric" > 0
 * - "Name" не является пустой строкой
 * - "Network" == true
 * - "NetworkFile" удалось успешно прочитать xml файл сети.
 * - "RootServer" для некорневого сервера необходимы адреса вышестоящего сервера.
 *
 * В случае ошибки \a m_network устанавливается в \a 0.
 */
void Daemon::link()
{
  m_numeric = quint8(m_settings->getInt("Numeric"));
  if (!m_numeric || m_settings->getString("Name").isEmpty() || !m_settings->getBool("Network")) {
    m_network = 0;

    if (m_settings->getBool("Network")) {
      QString reason;
      if (m_numeric)
        reason = tr("Пустое имя сервера");
      else
        reason = tr("Номер сервера равен 0");

      LOG(0, tr("- Error - Ошибка инициализации поддержки сети, [%1]").arg(reason));
    }

    return;
  }

  m_network = new Network(qApp->applicationDirPath());
  m_network->setSingle(true);
  if (!m_network->fromFile(m_settings->getString("NetworkFile"))) {
    LOG(0, tr("- Error - Ошибка инициализации поддержки сети, [%1: %2]").arg(m_settings->getString("NetworkFile")).arg(m_network->error()));
    delete m_network;
    m_network = 0;
  }
  else {
    m_profile = new AbstractProfile(this);
    m_profile->setNick(QString().number(m_settings->getInt("Numeric")));
    m_profile->setFullName(m_network->key());
    m_profile->setByeMsg(m_settings->getString("Name"));
    m_numerics << m_numeric;

    if (!m_settings->getBool("RootServer")) {
      if (m_network->count() > 0) {
        m_link = new ClientService(m_profile, m_network, this);
        connect(m_link, SIGNAL(newLink(quint8, const QString &, const QString &)), SLOT(newLink(quint8, const QString &, const QString &)));
        connect(m_link, SIGNAL(linkLeave(quint8, const QString &, const QString &)), SLOT(linkLeave(quint8, const QString &, const QString &)));
        connect(m_link, SIGNAL(relayMessage(const QString &, const QString &, const QString &)), SLOT(relayMessage(const QString &, const QString &, const QString &)));
        connect(m_link, SIGNAL(syncNumerics(const QList<quint8> &)), SLOT(syncNumerics(const QList<quint8> &)));
        connect(m_link, SIGNAL(newUser(const QStringList &, quint8, quint8)), SLOT(clientSyncUsers(const QStringList &, quint8, quint8)));
        connect(m_link, SIGNAL(accessGranted(const QString &, const QString &, quint16)), SLOT(clientAccessGranted(const QString &, const QString &, quint16)));
        connect(m_link, SIGNAL(syncUsersEnd()), SLOT(clientSyncUsersEnd()));
        connect(m_link, SIGNAL(unconnected(bool)), SLOT(clientServiceLeave(bool)));
        connect(m_link, SIGNAL(userLeave(const QString &, const QString &, quint8)), SLOT(clientUserLeave(const QString &, const QString &, quint8)));
        connect(m_link, SIGNAL(newNick(quint8, const QString &, const QString &, const QString &)), SLOT(syncProfile(quint8, const QString &, const QString &, const QString &)));
        connect(m_link, SIGNAL(syncBye(const QString &, const QString &)), SLOT(syncBye(const QString &, const QString &)));
        m_link->connectToHost();
        ServerInfo serverInfo = m_network->server();
        LOG(0, tr("- Notice - Инициализировано соединение с корневым сервером, %1:%2").arg(serverInfo.address).arg(serverInfo.port));
      }
      else {
        delete m_network;
        delete m_profile;
        m_network = 0;
        LOG(0, tr("- Error - Ошибка инициализации поддержки сети, [Не указан адрес вышестоящего сервера]"));
        return;
      }
    }

    LOG(0, tr("- Notice - Поддержка сети успешно инициализирована, %1@%2 \"%3\"").arg(m_numeric).arg(m_settings->getString("Name")).arg(m_network->name()));
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


/*!
 * \brief Отправка подключившемуся клиенту списка всех пользователей.
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


/*!
 * \brief Универсальная функция для обработки изменения сообщения о выходе пользователем.
 *
 * \param nick Ник пользователя.
 * \param bye Новое сообщение о выходе.
 * \param local Флаг локального подключения \a true локальное \a false удалённое.
 */
void Daemon::syncBye(const QString &nick, const QString &bye, bool local)
{
  if (!m_users.contains(nick))
    return;

  m_users.value(nick)->profile()->setByeMsg(bye);

  if (m_network) {
    emit sendSyncBye(nick, bye);

    if (m_remoteNumeric && local)
      m_link->sendSyncBye(nick, bye);
  }
}


/*!
 * \brief Универсальная функция для обработки изменения профиля и/или ника пользователя.
 *
 * \param gender Новый пол пользователя.
 * \param nick Старый ник пользователя.
 * \param nNick Новый ник пользователя, может быть пустой строкой, что означает что ник не изменился.
 * \param name Новое полное имя пользователя.
 * \param local Флаг локального подключения \a true локальное \a false удалённое.
 */
void Daemon::syncProfile(quint8 gender, const QString &nick, const QString &nNick, const QString &name, bool local)
{
  if (!m_users.contains(nick))
    return;

  if (nNick.isEmpty()) {
    if (m_settings->getBool("ChannelLog"))
      if (gender)
        m_channelLog->msg(tr("`%1` изменила свой профиль").arg(nick));
      else
        m_channelLog->msg(tr("`%1` изменил свой профиль").arg(nick));

    UserUnit *unit = m_users.value(nick);
    unit->profile()->setGender(gender);
    unit->profile()->setFullName(name);
    emit sendNewProfile(gender, nick, name);

    if (m_network) {
      emit sendSyncProfile(gender, nick, "", name);
      if (m_remoteNumeric && local)
        m_link->sendSyncProfile(gender, nick, "", name);
    }
  }
  else if (!m_users.contains(nNick)) {
    if (m_settings->getBool("ChannelLog"))
      if (gender)
        m_channelLog->msg(tr("`%1` теперь известна как `%2`").arg(nick).arg(nNick));
      else
        m_channelLog->msg(tr("`%1` теперь известен как `%2`").arg(nick).arg(nNick));

    UserUnit *unit = m_users.value(nick);
    m_users.remove(nick);
    m_users.insert(nNick, unit);
    unit->profile()->setGender(gender);
    unit->profile()->setNick(nNick);
    unit->profile()->setFullName(name);

    emit sendNewNick(gender, nick, nNick, name);

    if (m_network) {
      emit sendSyncProfile(gender, nick, nNick, name);
      if (m_remoteNumeric && local)
        m_link->sendSyncProfile(gender, nick, nNick, name);
    }
  }
  else if (local && m_users.contains(nNick)) {
    DaemonService *service = qobject_cast<DaemonService *>(sender());
    if (service)
      service->quit();
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

    if (m_maxUsersPerIp > 0) {
      QString ip = unit->profile()->host();
      if (m_ipLimits.contains(ip)) {
        int hosts = m_ipLimits.value(ip);
        if (hosts)
          m_ipLimits.insert(ip, hosts - 1);
        else
          m_ipLimits.remove(ip);
      }
    }

    LOG(0, tr("- Notice - Disconnect: %1@%2").arg(nick).arg(unit->profile()->host()));

    QString bye = unit->profile()->byeMsg();
    if (m_settings->getBool("ChannelLog"))
      if (unit->profile()->isMale())
        m_channelLog->msg(tr("`%1` вышел из чата: %2").arg(nick).arg(bye));
      else
        m_channelLog->msg(tr("`%1` вышла из чата: %2").arg(nick).arg(bye));

    emit userLeave(nick, bye, 1);

    if (m_network && m_remoteNumeric && unit->numeric() == m_numeric)
      m_link->sendUserLeave(nick, bye, 1);

    delete unit;
  }
}
