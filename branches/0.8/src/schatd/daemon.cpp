/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include "abstractprofile.h"
#include "channellog.h"
#include "clientservice.h"
#include "daemon.h"
#include "daemonlog.h"
#include "daemonservice.h"
#include "daemonsettings.h"
#include "linkunit.h"
#include "normalizereader.h"
#include "protocol.h"
#include "text/PlainTextFilter.h"
#include "translation.h"
#include "userunit.h"
#include "version.h"

#ifndef SCHAT_NO_LOCAL_SERVER
  #include "ipc/localservice.h"
#endif

#ifdef SCHAT_DEBUG
  #undef SCHAT_DEBUG
  #define SCHAT_DEBUG(x) qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << x;
  #include <QDebug>
  #include <QTime>
#else
  #define SCHAT_DEBUG(x)
#endif

Daemon *Daemon::m_self = 0;

/*!
 * \brief Конструктор класса Daemon.
 * \param parent Указатель на родительский объект.
 */
Daemon::Daemon(QObject *parent)
  : QObject(parent),
  m_syncUsers(false),
  m_channelLog(0),
  m_privateLog(0),
  m_statsInterval(0),
  m_remoteNumeric(0)
{
  m_self = this;
  environment();
  m_settings = new DaemonSettings(m_environment.value(EnvConfFile), this);

  m_translation = new Translation(this);
  m_translation->setPrefix("schatd_");
  m_translation->setSearch(m_environment.value(EnvShare) + "/translations");

  zombieTimer.setInterval(30000);
  connect(&m_server, SIGNAL(newConnection()), SLOT(incomingConnection()));
  connect(this, SIGNAL(newUser(const QStringList &, quint8, quint8)), SLOT(logNewUser(const QStringList &, quint8, quint8)));
  connect(this, SIGNAL(sendNewLink(quint8, const QString &, const QString &)), SLOT(logNewLink(quint8, const QString &, const QString &)));
  connect(this, SIGNAL(sendLinkLeave(quint8, const QString &, const QString &)), SLOT(logLinkLeave(quint8, const QString &, const QString &)));
  connect(this, SIGNAL(sendMessage(const QString &, const QString &)), SLOT(logMessage(const QString &, const QString &)));
  connect(&zombieTimer, SIGNAL(timeout()), SLOT(detectZombie()));

  QFile pidfile(m_environment.value(EnvPidFile));
  if (pidfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&pidfile);
    out << QCoreApplication::applicationPid();
    pidfile.close();
  }
}


Daemon::~Daemon()
{
  QFile::remove(m_environment.value(EnvPidFile));
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
  zombieTimer.start();
  m_settings->read();
  m_translation->load(m_settings->getString("Translation"));

  logLevel = m_settings->getInt("LogLevel");
  if (logLevel > -1) {
    m_log = new DaemonLog(this);
    if (!m_log->init(m_environment.value(EnvLogDir) + "/schatd.log")) {
      m_log->deleteLater();
      logLevel = -1;
    }
  }

  if (m_settings->getBool("ChannelLog")) {
    m_channelLog = new ChannelLog(m_environment.value(EnvLogDir), this);
    m_channelLog->setChannel("#main");
    m_channelLog->setMode(ChannelLog::Plain);
  }

  if (m_settings->getBool("PrivateLog")) {
    m_privateLog = new ChannelLog(m_environment.value(EnvLogDir), this);
    m_privateLog->setChannel("#private");
    m_privateLog->setMode(ChannelLog::Plain);
  }

  m_maxLinks      = m_settings->getInt("MaxLinks");
  m_maxUsers      = m_settings->getInt("MaxUsers");
  m_maxUsersPerIp = m_settings->getInt("MaxUsersPerIp");

  #ifndef SCHAT_NO_LOCAL_SERVER
    if (m_settings->getBool("LocalServer")) {
      QString serverName = QCryptographicHash::hash(QCoreApplication::applicationDirPath().toUtf8(), QCryptographicHash::Md5).toHex();
      #ifdef Q_OS_UNIX
       #if QT_VERSION >= 0x040500
        QLocalServer::removeServer(serverName);
       #else
        QString fullServerName = QDir::tempPath() + QLatin1String("/") + serverName;
        if (QFile::exists(fullServerName))
          QFile::remove(fullServerName);
       #endif
      #endif
      m_localServer = new QLocalServer(this);
      if (m_localServer->listen(serverName))
        connect(m_localServer, SIGNAL(newConnection()), SLOT(incomingLocalConnection()));
      else
        m_localServer->deleteLater();
    }
  #endif /*SCHAT_NO_LOCAL_SERVER*/

  QString address = m_settings->getString("ListenAddress");
  quint16 port    = quint16(m_settings->getInt("ListenPort"));
  bool result     = m_server.listen(QHostAddress(address), port);

  if (result) {
    SCHATD_LOG(0, DaemonLog::Notice, tr("\"IMPOMEZIA Simple Chat Daemon\" has been successfully started up, address %1, port %2").arg(address).arg(port));
    link();
  }
  else {
    SCHATD_LOG(0, DaemonLog::Error, tr("\"IMPOMEZIA Simple Chat Daemon\", start up error [%1]").arg(m_server.errorString()));
  }

  reload(-1);
  m_stats = initStats();

  return result;
}


/**
 * Мягкий перезапуск сервера.
 */
void Daemon::reload(int code)
{
  Q_UNUSED(code)

  if (code != -1)
    m_settings->reload();

  NormalizeReader reader(m_normalize);
  if (!reader.readFile(envConfFile("NormalizeFile"))) {
    SCHATD_LOG(0, DaemonLog::Warning, tr("Open file failed: %1, the normalization of nick operates within the limited mode")
                .arg(envConfFile("NormalizeFile")));
  }
  m_motd = initMotd();
  initMigrate();

  m_floodLimits = FloodLimits(m_settings->getInt("FloodDetectTime"),
      m_settings->getInt("FloodLimit"),
      m_settings->getInt("JoinFloodBanTime"),
      m_settings->getInt("JoinFloodDetectTime"),
      m_settings->getInt("JoinFloodLimit"),
      m_settings->getInt("MaxRepeatedMsgs"),
      m_settings->getInt("MuteTime"));

  if (code != -1) {
    m_floodOffline.clear();

    QHashIterator<QString, UserUnit *> i(m_users);
    while (i.hasNext()) {
      i.next();
      if (i.value()->service())
        i.value()->setFloodLimits(m_floodLimits);
    }
    SCHATD_LOG(0, DaemonLog::Notice, tr("Soft restart of the server completed"));
  }
}


/*!
 * Уведомление об успешном установлении связи с вышестоящим сервером.
 *
 * Функция устанавливает номер вышестоящего сервера и добавляет его номер в список номеров.
 * \param network Параметр игнорируется и не используется.
 * \param server  Адрес вышестоящего сервера.
 * \param numeric Номер вышестоящего сервера.
 */
void Daemon::clientAccessGranted(const QString &network, const QString &server, quint16 numeric)
{
  Q_UNUSED(network)

  m_remoteNumeric = numeric;
  m_numerics << numeric;

  SCHATD_LOG(0, DaemonLog::Notice, tr("Connected to the root server: %1@%2").arg(numeric).arg(server));
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
      removeUser(i.value()->profile()->nick(), tr("The connection to the root server has been lost"), 0);
  }
}


/*!
 * \brief Синхронизация события подключения к удалённому серверу нового пользователя.
 *
 * Слот вызывается при получении пакета \b OpcodeNewUser от локального клиентского подключения либо от удалённого сервера.
 * Функция вносит нового пользователя в список \a m_users и высылает уведомление.
 *
 * \param list    Стандартный список, содержащий в себе полные данные пользователя.
 * \param echo    Флаг пакетной передачи.
 * \param numeric Номер сервера, к которому подключен данный пользователь.
 */
void Daemon::clientSyncUsers(const QStringList &list, quint8 echo, quint8 numeric)
{
  QString nick = normalizeNick(list.at(AbstractProfile::Nick));

  if (m_users.contains(nick) && !m_syncUsers) {
    DaemonService *service = m_users.value(nick)->service();
    if (service)
      service->quit(true);

    removeUser(list.at(AbstractProfile::Nick));
  }

  if (!m_users.contains(nick)) {
    m_users.insert(nick, new UserUnit(list, m_floodLimits, 0, numeric));
    emit newUser(list, echo, numeric);
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
    QMultiHash<quint32, QString> status;

    QHashIterator<QString, UserUnit *> i(m_users);
    while (i.hasNext()) {
      i.next();
      quint8 numeric = i.value()->numeric();
      if (numeric == m_numeric || m_links.contains(numeric)) {
        AbstractProfile *profile = i.value()->profile();

        if (profile->status())
          status.insert(profile->status(), profile->nick());

        m_link->sendNewUser(profile->pack(), 0, numeric);
      }
    }

    if (!status.isEmpty()) {
      QList<quint32> statusCodes;
      QHashIterator<quint32, QString> i(status);
      while (i.hasNext()) {
        i.next();
        if (!statusCodes.contains(i.key()))
          statusCodes << i.key();
      }
      foreach (quint32 code, statusCodes) {
        m_link->sendUniversal(schat::UniStatusList, QList<quint32>() << code, status.values(code));
      }
    }
  }

  m_syncUsers = true;
}


/*!
 * Синхронизация события отключения пользователя от удалённого сервера.
 * Событие игнорируется если пользователь является локальным.
 *
 * Функция устанавливает в профиле пользователя новое сообщение о выходе и вызывает функцию userLeave(const QString &nick).
 * \param nick Ник пользователя.
 * \param bye  Сообщение о выходе.
 * \param flag Параметр игнорируется и не используется.
 * \sa userLeave(const QString &nick, const QString &err)
 */
void Daemon::clientUserLeave(const QString &nick, const QString &bye, quint8 flag)
{
  QString lowerNick = normalizeNick(nick);

  if (m_users.contains(lowerNick)) {
    UserUnit *unit = m_users.value(lowerNick);
    if (unit->numeric() == m_numeric)
      return;

    unit->profile()->setByeMsg(bye);
  }

  removeUser(nick, tr("The user is disconnected from the remote server"), flag);
}


/*!
 * Определение локальных зависших пользователей.
 */
void Daemon::detectZombie()
{
  if (!m_users.isEmpty()) {
    foreach (UserUnit *unit, m_users) {
      if (unit->numeric() == m_numeric && !unit->service())
        removeUser(unit->profile()->nick(), "Detect zombie");
    }
  }
}


void Daemon::dumpStats()
{
  QFile file(m_statsFile);
  if (!file.open(QFile::WriteOnly | QFile::Text))
    return;

  QXmlStreamWriter writer;
  writer.setAutoFormatting(true);
  writer.setAutoFormattingIndent(2);
  writer.setDevice(&file);
  writer.writeStartDocument();

  writer.writeStartElement("stats");
  writer.writeAttribute("version", "1.0");
  writer.writeTextElement("users", QString::number(m_users.count()));
  writer.writeTextElement("servers", QString::number(m_numerics.count()));
  writer.writeEndDocument();
}


/*!
 * \brief Обработка приветствия от подключенного клиента.
 *
 * В зависимости от значения \a flag вызывается функция greetingLink(const QStringList &list, DaemonService *service) для сервера
 * либо greetingUser(const QStringList &list, DaemonService *service) для клиента.
 *
 * \param list Стандартный список, содержащий в себе полные данные пользователя.
 * \param flag Флаг подключения.
 */
void Daemon::greeting(const QStringList &list, quint8 flag)
{
  SCHAT_DEBUG(this << "::greeting()" << list.at(AbstractProfile::Nick) << flag << sender())

  DaemonService *service = qobject_cast<DaemonService *>(sender());
  if (service) {
    if (flag == FlagLink) {
      quint16 err = greetingLink(list, service);
      if (err) {
        SCHATD_LOG(0, DaemonLog::Warning, tr("Access is denied for the server: %1@%2, error code: %3")
            .arg(QString(list.at(AbstractProfile::Nick)))
            .arg(list.at(AbstractProfile::Host))
            .arg(err));
        service->accessDenied(err);
      }
    }
    else {
      quint16 err = greetingUser(list, service);
      if (err) {
        SCHATD_LOG(0, DaemonLog::Warning, tr("Access is denied for the user: %1@%2, error code: %3")
            .arg(QString(list.at(AbstractProfile::Nick)))
            .arg(list.at(AbstractProfile::Host))
            .arg(err));
        service->accessDenied(err);
      }
    }
  }
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
    connect(service, SIGNAL(leave(const QString &, quint8, const QString &)), SLOT(serviceLeave(const QString &, quint8, const QString &)));
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
 * Запись в канальный журнал события отключения сервера.
 */
void Daemon::logLinkLeave(quint8 /*numeric*/, const QString &network, const QString &name)
{
  if (m_channelLog)
    m_channelLog->msg(tr("Server \"%1\" disconnected from network \"%2\"").arg(name).arg(network));
}


/*!
 * Запись в канальный журнал сообщения в главный канал.
 */
void Daemon::logMessage(const QString &sender, const QString &message)
{
  if (m_channelLog)
    m_channelLog->msg(QString("%1: %2").arg(sender).arg(PlainTextFilter::filter(message)));
}


/*!
 * Запись в канальный журнал события подключения сервера.
 */
void Daemon::logNewLink(quint8 /*numeric*/, const QString &network, const QString &name)
{
  if (m_channelLog)
    m_channelLog->msg(tr("Server \"%1\" connected to network \"%2\"").arg(name).arg(network));
}


/*!
 * Запись в журналы сервера события подключения нового пользователя.
 */
void Daemon::logNewUser(const QStringList &list, quint8 /*echo*/, quint8 numeric)
{
  QString nick = list.at(AbstractProfile::Nick);

  SCHATD_LOG(0, DaemonLog::Notice, tr("Connected [%1]: %2@%3, %4, %5, %6")
      .arg(numeric)
      .arg(nick)
      .arg(list.at(AbstractProfile::Host))
      .arg(list.at(AbstractProfile::FullName))
      .arg(list.at(AbstractProfile::Gender))
      .arg(list.at(AbstractProfile::UserAgent)));

  if (m_channelLog) {
    if (list.at(AbstractProfile::Gender) == "male")
      m_channelLog->msg(tr("\"%1\" entered chat", "Male").arg(nick));
    else
      m_channelLog->msg(tr("\"%1\" entered chat", "Female").arg(nick));
  }
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
  SCHAT_DEBUG(this << "::message()" << channel << nick << msg)

  // Проверка сообщения на флуд.
  UserUnit *user = m_users.value(normalizeNick(nick), 0);
  if (!user)
    return;

  int mute = user->isFlood(channel, msg);
  if (mute > 0) {
    DaemonService *service = user->service();
    if (service) {
      service->sendServerMessage(tr("To prevent flood, you muted to <b>%n</b> seconds", "", mute));
    }
    return;
  }

  QString lowerChannel = normalizeNick(channel);

  if (channel.isEmpty()) {
    if (!parseCmd(nick, msg)) {
      emit sendMessage(nick, msg);
      if (m_network) {
        emit sendRelayMessage(channel, nick, msg);
        if (m_remoteNumeric)
          m_link->sendRelayMessage(channel, nick, msg);
      }
    }
  }
  else if (m_users.contains(lowerChannel)) {
    if (m_privateLog)
      m_privateLog->msg(QString("\"%1\" -> \"%2\": %3").arg(nick).arg(channel).arg(PlainTextFilter::filter(msg)));

    if (!parseCmd(nick, msg)) {
      quint16 numeric = m_users.value(lowerChannel)->numeric();
      DaemonService *senderService = qobject_cast<DaemonService *>(sender());
      if (!senderService)
        return;
      bool err = true;

      if (numeric == m_numeric) {
        err = false;
        DaemonService *service = m_users.value(lowerChannel)->service();
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
 * Если \a channel пустая строка, то это сообщение предназначено для отправки в главный канал.
 * Происходит рассылка уведомление для локальных клиентов и остальных серверов,
 * при необходимости добавляется запись в канальный лог.
 *
 * Отправка приватных сообщений произовдится различными способами в зависимости от того к какому серверу подключен получатель. Получатель может быть:
 *  - Подключенным локально к этому серверу (numeric == m_numeric).
 *  - Подключенным к одному из серверов подкюченных к данному серверу (m_links.contains(numeric)).
 *
 * \param channel Канал/ник для кого предназначено сообщение (пустая строка - главный канал).
 * \param sender  Ник отправителся.
 * \param msg     Cообщение.
 */
void Daemon::relayMessage(const QString &channel, const QString &sender, const QString &msg)
{
  SCHAT_DEBUG(this << "::relayMessage()" << channel << sender << msg)

  if (!m_network)
    return;

  QString lowerChannel = normalizeNick(channel);
  QString lowerSender  = normalizeNick(sender);

  if (channel.isEmpty()) {
    emit sendMessage(sender, msg);

    if (m_users.contains(lowerSender)) {
      quint8 senderNum = m_users.value(lowerSender)->numeric();

      QHashIterator<quint8, LinkUnit *> i(m_links);
      while (i.hasNext()) {
        i.next();
        if (i.key() != senderNum && i.value()->service())
          i.value()->service()->sendRelayMessage("", sender, msg);
      }
    }
    return;
  }

  quint8 numeric = m_users.value(lowerChannel)->numeric();

  if (m_privateLog)
    m_privateLog->msg(QString("\"%1\" -> \"%2\": %3").arg(sender).arg(channel).arg(PlainTextFilter::filter(msg)));

  if (numeric == m_numeric) {
    DaemonService *service = m_users.value(lowerChannel)->service();
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
 * \brief Обработка отключения авторизированного пользователя.
 *
 * В зависимости от значения flag вызывается функция linkLeave(const QString &nick) для сервера либо userLeave(const QString &nick, const QString &err) для пользователей.
 * \param nick Ник пользователя.
 * \param flag Флаг подключения.
 * \param err Ошибка, причина разъединения.
 * \sa linkLeave(const QString &nick, const QString &err), userLeave(const QString &nick, const QString &err)
 */
void Daemon::serviceLeave(const QString &nick, quint8 flag, const QString &err)
{
  SCHAT_DEBUG(this << "::serviceLeave()" << nick << flag)

  if (flag == FlagLink)
    linkLeave(nick, err);
  else
    removeUser(nick, err);
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


/*!
 * Получение универсального пакета от вышестоящего сервера.
 * или если \p numeric > 0, то от вторичного.
 *
 * Поддерживаемые форматы пакетов: schat::UniStatusList.
 */
void Daemon::universal(quint16 sub, const QList<quint32> &data1, const QStringList &data2, quint8 numeric)
{
  if (sub == schat::UniStatusList && !data1.isEmpty() && !data2.isEmpty()) {
    updateStatus(data1.at(0), data2);

    if (numeric) {
      QHashIterator<quint8, LinkUnit *> i(m_links);
      while (i.hasNext()) {
        i.next();
        if (i.key() != numeric && i.value()->service())
          i.value()->service()->sendUniversal(sub, data1, data2);
      }
    }
    emit sendUniversal(sub, data1, data2);
  }
}


/*!
 * Получение универсального пакета от локального пользователя.
 * Пакет обрабатывается и вызывается сигнал sendUniversal(quint16 sub, const QList<quint32> &data1, const QStringList &data2)
 * для отправки нового универсального пакета для локальных клиентов.
 * В случае поддержки сети пакет также отправляется на корневой сервер или на все вторичные сервера.
 *
 * Поддерживаемые форматы пакетов: schat::UniStatus.
 */
void Daemon::universal(quint16 sub, const QString &nick, const QList<quint32> &data1, const QStringList &data2)
{
  Q_UNUSED(data2)

  QString lowerNick = normalizeNick(nick);

  if (m_users.contains(lowerNick)) {
    if (sub == schat::UniStatus && !data1.isEmpty() && data1.at(0) != schat::StatusOffline) {
      m_users.value(lowerNick)->profile()->setStatus(data1.at(0));
      QList<quint32> out1;
      out1 << data1.at(0) << 1;
      emit sendUniversal(schat::UniStatusList, out1, QStringList(nick));

      if (m_network) {
        if (m_remoteNumeric)
          m_link->sendUniversal(schat::UniStatusList, out1, QStringList(nick));
        else {
          foreach (LinkUnit *unit, m_links)
            if (unit->service())
              unit->service()->sendUniversal(schat::UniStatusList, out1, QStringList(nick));
        }
      }
    }
  }
}


#ifndef SCHAT_NO_LOCAL_SERVER
void Daemon::incomingLocalConnection()
{
  if (m_localServer->hasPendingConnections()) {
    new LocalService(m_localServer->nextPendingConnection(), this);
  }
}
#endif /*SCHAT_NO_LOCAL_SERVER*/


bool Daemon::initMigrate()
{
  QFile file(envConfFile("MigrateFile"));
  if (file.exists() && file.open(QFile::ReadOnly)) {
    m_migrate = file.readAll();
    return true;
  }

  return false;
}


/*!
 * Инициализирует поддержку команды /motd (Message Of The Day).
 *
 * \return Возвращает \a true в случае успеха.
 */
bool Daemon::initMotd()
{
  int size = m_settings->getInt("MotdMaxSize");

  if (!m_settings->getBool("Motd") || size < 1)
    return false;

  QFile file(envConfFile("MotdFile"));
  if (!file.exists())
    return false;

  if (file.open(QFile::ReadOnly | QFile::Text)) {
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    m_motdText = stream.read(size);
    if (!m_motdText.isEmpty()) {
      m_motdText.replace("${VERSION}", SCHAT_VERSION);
      if (m_network)
        m_motdText.replace("${NETWORK}", m_network->name());

      return true;
    }
  }

  return false;
}


/*!
 * Инициализирует поддержку записи статистической информации.
 */
bool Daemon::initStats()
{
  if (m_settings->getBool("Stats")) {
    m_statsInterval = m_settings->getInt("StatsInterval");
    if (m_statsInterval < 1)
      m_statsInterval = 1;

    m_statsFile = m_settings->getString("StatsFile");
    if (QFileInfo(m_statsFile).isRelative())
      m_statsFile = m_environment.value(EnvVarDir) + '/' + m_statsFile;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(dumpStats()));
    timer->start(m_statsInterval * 1000);

    return true;
  }

  return false;
}


/*!
 * \brief Обработка команд предназначенных для сервера.
 *
 * \param nick Ник пользователя отправившего сообщение.
 * \param msg  Сообщение.
 * \return \a true если команда опознана и выполнена, \a false при возникновении любой ошибки.
 */
bool Daemon::parseCmd(const QString &nick, const QString &msg)
{
  QString lowerNick = normalizeNick(nick);

  if (!m_users.contains(lowerNick))
    return false;

  DaemonService *service = m_users.value(lowerNick)->service();
  if (!service)
    return false;

  QString text = PlainTextFilter::filter(msg).toLower();

  /// Команда "/server"
  if (text == "/server") {
    service->sendServerMessage(serverInfo());
    return true;
  }
  else if (text == "/motd") {
    if (m_motd) {
      QString motdText = m_motdText;
      motdText.replace("${USERS_NUM}", QString::number(m_users.count()));
      motdText.replace("${USERS}", tr("<b>%n</b> users", "", m_users.count()));
      motdText.replace("${SERVERS_NUM}", QString::number(m_numerics.count()));
      motdText.replace("${SERVERS}", tr("<b>%n</b> servers", "", m_numerics.count()));
      service->sendServerMessage(motdText);
    }
    return true;
  }
  else if (text == "/ping") {
    service->sendServerMessage("/pong");
    return true;
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
 * Формирует полный путь к файлу, если задан относительный,
 * с учётом расположения конфигурационного файла.
 *
 * \param key Ключ в настройках.
 */
QString Daemon::envConfFile(const QString &key) const
{
  QString file = m_settings->getString(key);
  if (QFileInfo(file).isRelative())
    file = QFileInfo(m_environment.value(EnvConfFile)).absolutePath() + "/" + file;

  return file;
}


/*!
 * Выполняет нормализацию ника, для использования его в качестве ключа
 * в таблице пользователей.
 */
QString Daemon::normalizeNick(const QString &nick) const
{
  if (nick.isEmpty())
    return "";

  QString out = nick.toLower();
  for (int i = 0; i < out.size(); ++i) {
    if (m_normalize.contains(out.at(i)))
      out[i] = m_normalize.value(out.at(i));
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
  QString info = QString("<b>IMPOMEZIA Simple Chat Daemon %1</b>, <a href='https://schat.me'>https://schat.me</a>"
      "<table><tr><td class='info'>%2 </td><td><b>").arg(SCHAT_VERSION).arg(tr("Platform:"));

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

  info += QString(", Qt %1</b></td></tr>").arg(qVersion());
  if (m_network) {
    info += QString("<tr><td class='info'>%1 </td><td><b>%2</b></td></tr>").arg(tr("Network name:")).arg(m_network->name());
    info += QString("<tr><td class='info'>%1 </td><td><b>%2</b></td></tr>").arg(tr("Server name:")).arg(m_settings->getString("Name"));
    info += QString("<tr><td class='info'>%1 </td><td><b>%2</b></td></tr>").arg(tr("Users:")).arg(m_users.count());
    info += QString("<tr><td class='info'>%1 </td><td><b>%2</b></td></tr></table>").arg(tr("Servers:")).arg(m_numerics.count());
  }
  else
    info += QString("<tr><td class='info'>%1 </td><td><b>%2</b></td></tr></table>").arg(tr("Users:")).arg(m_users.count());

  return info;
}


/*!
 * Обработка приветствия от удалённого сервера.
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
 * \param list    Стандартный список, содержащий в себе полные данные пользователя.
 * \param service Указатель на сервис.
 */
quint16 Daemon::greetingLink(const QStringList &list, DaemonService *service)
{
  if (!m_network)
    return ErrorNotNetworkConfigured;

  if (m_remoteNumeric)
    return ErrorRootServerIsSlave;

  if (m_network->key() != list.at(AbstractProfile::FullName))
    return ErrorBadNetworkKey;

  quint8 numeric = quint8(QString(list.at(AbstractProfile::Nick)).toInt());
  if (m_numerics.contains(numeric))
    return ErrorNumericAlreadyUse;

  if (m_maxLinks > 0)
    if (m_maxLinks == localLinksCount())
      return ErrorLinksLimitExceeded;

  m_links.insert(numeric, new LinkUnit(list.at(AbstractProfile::ByeMsg), service));
  m_numerics << numeric;
  connect(service, SIGNAL(newNick(quint8, const QString &, const QString &, const QString &)), SLOT(syncProfile(quint8, const QString &, const QString &, const QString &)));
  connect(service, SIGNAL(relayMessage(const QString &, const QString &, const QString &)), SLOT(relayMessage(const QString &, const QString &, const QString &)));
  connect(service, SIGNAL(newUser(const QStringList &, quint8, quint8)), SLOT(clientSyncUsers(const QStringList &, quint8, quint8)));
  connect(service, SIGNAL(userLeave(const QString &, const QString &, quint8)), SLOT(clientUserLeave(const QString &, const QString &, quint8)));
  connect(service, SIGNAL(newBye(const QString &, const QString &)), SLOT(syncBye(const QString &, const QString &)));
  connect(service, SIGNAL(universal(quint16, const QList<quint32> &, const QStringList &, quint8)), SLOT(universal(quint16, const QList<quint32> &, const QStringList &, quint8)));
  connect(this, SIGNAL(sendRelayMessage(const QString &, const QString &, const QString &)), service, SLOT(sendRelayMessage(const QString &, const QString &, const QString &)));
  connect(this, SIGNAL(sendSyncProfile(quint8, const QString &, const QString &, const QString &)), service, SLOT(sendNewNick(quint8, const QString &, const QString &, const QString &)));
  connect(this, SIGNAL(sendSyncBye(const QString &, const QString &)), service, SLOT(sendSyncBye(const QString &, const QString &)));
  service->accessGranted(m_numeric, m_migrate);
  service->sendNumerics(m_numerics);

  SCHATD_LOG(0, DaemonLog::Notice, tr("Connected server: %1@%2, %3").arg(numeric).arg(list.at(AbstractProfile::Host)).arg(list.at(AbstractProfile::UserAgent)));
  emit sendNewLink(numeric, m_network->name(), list.at(AbstractProfile::ByeMsg));
  sendAllUsers(service);

  return 0;
}


/*!\
 * \brief Обработка приветствия от локально подключенного пользователя.
 *
 * \param list    Стандартный список, содержащий в себе полные данные пользователя.
 * \param service Указатель на сервис.
 */
quint16 Daemon::greetingUser(const QStringList &list, DaemonService *service)
{
  QString nick = normalizeNick(list.at(AbstractProfile::Nick));

  if (m_maxUsers > 0)
    if (m_maxUsers == localUsersCount())
      return ErrorUsersLimitExceeded;

  if (m_maxUsersPerIp > 0) {
    QString ip = list.at(AbstractProfile::Host);
    if (m_ipLimits.contains(ip)) {
      int hosts = m_ipLimits.value(ip);
      if (hosts == m_maxUsersPerIp) {
        return ErrorMaxUsersPerIpExceeded;
      }
      else {
        m_ipLimits.insert(ip, hosts + 1);
      }
    }
    else {
      m_ipLimits.insert(ip, 1);
    }
  }

  // Восстановление сохранённых флуд лимитов.
  UserUnit *user = new UserUnit(list, m_floodLimits, service, m_numeric);
  if (m_floodOffline.contains(nick) && service->host() == m_floodOffline.value(nick).host()) {
    user->setMuteTime(m_floodOffline.value(nick).muteTime());

    FloodOfflineItem item = m_floodOffline.value(nick);
    if (QDateTime::currentDateTime().toTime_t() - item.timeStamp() < (uint) m_floodLimits.joinFloodBanTime()) {
       if (item.reconnects() >= m_floodLimits.joinFloodLimit()) {
         delete user;
         return ErrorUsersLimitExceeded;
       }

      user->setReconnects(item.reconnects() + 1);
    }
  }

  if (m_users.contains(nick)) {
    UserUnit *unit = m_users.value(nick);
    if (unit->numeric() == m_numeric && !unit->service())
      removeUser(unit->profile()->nick(), "Detect zombie");

    if (m_floodOffline.contains(nick))
      m_floodOffline.remove(nick);

    m_floodOffline.insert(nick, FloodOfflineItem(user->reconnects(), user->profile()->host(), user->muteTime(), QDateTime::currentDateTime().toTime_t()));
    delete user;
    return ErrorNickAlreadyUse;
  }

  m_users.insert(nick, user);
  connect(service, SIGNAL(newNick(quint8, const QString &, const QString &, const QString &)), SLOT(newNick(quint8, const QString &, const QString &, const QString &)));
  connect(service, SIGNAL(newProfile(quint8, const QString &, const QString &)), SLOT(newProfile(quint8, const QString &, const QString &)));
  connect(service, SIGNAL(newBye(const QString &, const QString &)), SLOT(newBye(const QString &, const QString &)));
  connect(service, SIGNAL(message(const QString &, const QString &, const QString &)), SLOT(message(const QString &, const QString &, const QString &)));
  connect(service, SIGNAL(universal(quint16, const QString &, const QList<quint32> &, const QStringList &)), SLOT(universal(quint16, const QString &, const QList<quint32> &, const QStringList &)));
  connect(this, SIGNAL(sendNewNick(quint8, const QString &, const QString &, const QString &)), service, SLOT(sendNewNick(quint8, const QString &, const QString &, const QString &)));
  connect(this, SIGNAL(sendNewProfile(quint8, const QString &, const QString &)), service, SLOT(sendNewProfile(quint8, const QString &, const QString &)));
  connect(this, SIGNAL(sendMessage(const QString &, const QString &)), service, SLOT(sendMessage(const QString &, const QString &)));
  connect(this, SIGNAL(sendUniversal(quint16, const QList<quint32> &, const QStringList &)), service, SLOT(sendUniversal(quint16, const QList<quint32> &, const QStringList &)));
  service->accessGranted(m_numeric, m_migrate);
  emit newUser(list, 1, m_numeric);

  sendAllUsers(service);
  if (m_network && m_remoteNumeric)
    m_link->sendNewUser(list, 1, m_numeric);

  return 0;
}


/*!
 * Получение значения системной переменной окружения.
 *
 * \param env      Имя переменной.
 * \param failBack Значение по умолчанию.
 * \return         Значение переменной или значение по умолчанию.
 */
QString Daemon::envValue(const QString &env, const QString &failBack)
{
  QStringList environment = QProcess::systemEnvironment();
  QString out = failBack;
  int index = environment.indexOf(QRegExp(env + "=.*"));
  if (index != -1) {
    QStringList list = environment.at(index).split("=");
    if (list.size() == 2)
      out = list.at(1);
  }
  return out;
}


/*!
 * Установка настроек, которые могут быть изменены через переменные окружения.
 */
void Daemon::environment()
{
  QString appDirPath = QCoreApplication::applicationDirPath();
  bool sbin = appDirPath == "/usr/sbin";

  m_environment.insert(EnvConfFile, envValue("SCHATD_CONF",  (sbin ? "/etc/schatd" : appDirPath) + "/schatd.conf"));
  m_environment.insert(EnvPidFile,  envValue("SCHATD_PID",   (sbin ? "/var/run/schatd" : appDirPath) + "/schatd.pid"));
  m_environment.insert(EnvLogDir,   envValue("SCHATD_LOG",   sbin ? "/var/log/schatd" : appDirPath + "/log"));
  m_environment.insert(EnvVarDir,   envValue("SCHATD_VAR",   sbin ? "/var/lib/schatd" : appDirPath));
  m_environment.insert(EnvShare,    envValue("SCHATD_SHARE", sbin ? "/usr/share/schatd" : appDirPath));
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

    if (m_settings->getBool("Network")) {
      QString reason;
      if (m_numeric)
        reason = tr("Empty server name");
      else
        reason = tr("Server number is 0");

      SCHATD_LOG(0, DaemonLog::Error, tr("Error initializing network support, [%1]").arg(reason));
    }

    return;
  }

  QString networkFile = m_settings->getString("NetworkFile");
  QString networkFilePath = m_environment.value(EnvConfFile);
  if (!QFileInfo(networkFile).isRelative()) {
    networkFilePath = networkFile;
    networkFile = QFileInfo(networkFile).fileName();
  }
  networkFilePath = QFileInfo(networkFilePath).absolutePath();

  m_network = new Network(QStringList(networkFilePath), this);
  m_network->setSingle(true);
  if (!m_network->fromFile(networkFile)) {
    SCHATD_LOG(0, DaemonLog::Error, tr("Error initializing network support, [%1: %2]").arg(m_settings->getString("NetworkFile")).arg(m_network->error()));
    delete m_network;
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
        connect(m_link, SIGNAL(universal(quint16, const QList<quint32> &, const QStringList &)), SLOT(universal(quint16, const QList<quint32> &, const QStringList &)));
        m_link->connectToHost();
        ServerInfo serverInfo = m_network->server();
        SCHATD_LOG(0, DaemonLog::Notice, tr("The connection to the root server is initialized %1:%2").arg(serverInfo.address).arg(serverInfo.port));
      }
      else {
        delete m_network;
        delete m_profile;
        SCHATD_LOG(0, DaemonLog::Error, tr("Error initializing network support, [no address of the parent server]"));
        return;
      }
    }

    SCHATD_LOG(0, DaemonLog::Notice, tr("Network support successfully initialized, %1@%2 \"%3\"").arg(m_numeric).arg(m_settings->getString("Name")).arg(m_network->name()));
    SCHAT_DEBUG(this << "network up!" << m_settings->getString("Name") << m_numeric);
  }
}


/*!
 * \brief Обработка отключения нижестоящего сервера.
 *
 * Сервер удаляется из списка подключенных серверов и его номер удаляется из списка номеров серверов.
 * Высылается уведомление об отключении сервера, и отключаются все клиенты, ассоциированные с этим сервером.
 * Событие записывается в журнал.
 *
 * \param nick Номер сервера в виде строки.
 * \param err Ошибка, причина разъединения.
 */
void Daemon::linkLeave(const QString &nick, const QString &err)
{
  if (m_network) {
    quint8 numeric = quint8(nick.toInt());

    if (m_links.contains(numeric)) {
      LinkUnit *unit = m_links.value(numeric);
      m_links.remove(numeric);
      m_numerics.removeAll(numeric);

      SCHATD_LOG(0, DaemonLog::Notice, tr("Server disconnected: %1@%2 [%3]").arg(nick).arg(unit->host()).arg(err));

      emit sendLinkLeave(numeric, m_network->name(), unit->host());

      QHashIterator<QString, UserUnit *> i(m_users);
      while (i.hasNext()) {
        i.next();
        if (i.value()->numeric() == numeric)
          removeUser(i.value()->profile()->nick(), tr("Server disconnected %1@%2").arg(nick).arg(unit->host()), 0);
      }

      delete unit;
    }
  }
}


/*!
 * \brief Обработка отключения пользователя.
 *
 * Все действия производятся, только если ник преобразованный в нижний регистр найден в \a m_users.
 * При использовании ограничений по количеству подключений с одного адреса, производится уменьшение счётчика адресов
 * или удаление записи в \a m_ipLimits.
 * Событие отключения записывается в журнал сервера и при необходимости в канальный лог.
 * Высылается сигнал userLeave(const QString &nick, const QString &bye, quint8 flag) и при наличии корневого сервера он
 * также уведомляется об отключении пользователя (при условии, что пользователь локальный).
 *
 * \param nick Ник пользователя.
 * \param err  Ошибка, причина разъединения.
 * \param flag Флаг пакетной передачи.
 */
void Daemon::removeUser(const QString &nick, const QString &err, quint8 flag)
{
  QString lowerNick = normalizeNick(nick);

  if (m_users.contains(lowerNick)) {
    UserUnit *user = m_users.value(lowerNick);

    m_users.remove(lowerNick);

    if (m_maxUsersPerIp > 0) {
      QString ip = user->profile()->host();
      if (m_ipLimits.contains(ip)) {
        int hosts = m_ipLimits.value(ip);
        if (hosts)
          m_ipLimits.insert(ip, hosts - 1);
        else
          m_ipLimits.remove(ip);
      }
    }

    SCHATD_LOG(0, DaemonLog::Notice, tr("Disconnect: %1@%2 [%3]").arg(nick).arg(user->profile()->host()).arg(err));

    QString bye = user->profile()->byeMsg();
    if (m_channelLog) {
      if (user->profile()->isMale())
        m_channelLog->msg(tr("\"%1\" left the chat: %2", "Male").arg(nick).arg(bye));
      else
        m_channelLog->msg(tr("\"%1\" left the chat: %2", "Female").arg(nick).arg(bye));
    }

    emit userLeave(nick, bye, flag);

    if (m_network && m_remoteNumeric && user->numeric() == m_numeric)
      m_link->sendUserLeave(nick, bye, flag);

    // При необходимости сохраняет время начала действия ограничения флуда.
    if (user->service() && (user->muteTime() || (QDateTime::currentDateTime().toTime_t() - user->timeStamp() < (uint) m_floodLimits.joinFloodDetectTime()))) {
      if (m_floodOffline.contains(lowerNick))
        m_floodOffline.remove(lowerNick);

      m_floodOffline.insert(lowerNick, FloodOfflineItem(user->reconnects(), user->profile()->host(), user->muteTime(), user->timeStamp()));
    }

    delete user;
  }
}


/*!
 * \brief Отправка подключившемуся клиенту списка всех пользователей.
 */
void Daemon::sendAllUsers(DaemonService *service)
{
  if (service) {
    QMultiHash<quint32, QString> status;

    QHashIterator<QString, UserUnit *> i(m_users);
    while (i.hasNext()) {
      i.next();
      AbstractProfile *profile = i.value()->profile();
      if (profile->status())
        status.insert(profile->status(), profile->nick());
      service->sendNewUser(profile->pack(), 0, i.value()->numeric());
    }

    if (!status.isEmpty()) {
      QList<quint32> statusCodes;
      QHashIterator<quint32, QString> i(status);
      while (i.hasNext()) {
        i.next();
        if (!statusCodes.contains(i.key()))
          statusCodes << i.key();
      }
      foreach (quint32 code, statusCodes) {
        service->sendUniversal(schat::UniStatusList, QList<quint32>() << code, status.values(code));
      }
    }

    service->sendSyncUsersEnd();
  }
}


/*!
 * \brief Универсальная функция для обработки изменения сообщения о выходе пользователем.
 *
 * \param nick  Ник пользователя.
 * \param bye   Новое сообщение о выходе.
 * \param local Флаг локального подключения \a true локальное \a false удалённое.
 */
void Daemon::syncBye(const QString &nick, const QString &bye, bool local)
{
  QString lowerNick = normalizeNick(nick);

  if (!m_users.contains(lowerNick))
    return;

  m_users.value(lowerNick)->profile()->setByeMsg(bye);

  if (m_network) {
    emit sendSyncBye(nick, bye);

    if (m_remoteNumeric && local)
      m_link->sendSyncBye(nick, bye);
  }
}


/*!
 * \brief Универсальная функция для обработки изменения профиля и/или ника пользователя.
 *
 * Все действия производятся, только если ник преобразованный в нижний регистр найден в \a m_users.
 *
 * Если новый ник является пустой строкой, это означает что ник не изменился а изменилась лишь профильная информация.
 * Если \a m_users не содержит нового ника приведённого к нижнему регистру или старый и новый ник в нижнем регистре равны,
 * производится изменение ника.
 * Иначе если пользователь локальный и новый ник в нижнем регистре присутствует в \a m_users, то происходит
 * попытка занять чужой ник, такой пользователь отключается.
 *
 * \param gender Новый пол пользователя.
 * \param nick   Старый ник пользователя.
 * \param nNick  Новый ник пользователя, может быть пустой строкой, что означает что ник не изменился.
 * \param name   Новое полное имя пользователя.
 * \param local  Флаг локального подключения \a true локальное \a false удалённое.
 */
void Daemon::syncProfile(quint8 gender, const QString &nick, const QString &nNick, const QString &name, bool local)
{
  SCHAT_DEBUG(this << "::syncProfile()" << gender << nick << nNick << name << local);

  QString lowerNick = normalizeNick(nick);

  if (!m_users.contains(lowerNick))
    return;

  QString lowerNewNick = normalizeNick(nNick);

  if (lowerNewNick.isEmpty()) {
    if (m_channelLog) {
      if (gender)
        m_channelLog->msg(tr("\"%1\" has changed his profile", "Female").arg(nick));
      else
        m_channelLog->msg(tr("\"%1\" has changed his profile", "Male").arg(nick));
    }

    UserUnit *unit = m_users.value(lowerNick);
    unit->profile()->setGender(gender);
    unit->profile()->setFullName(name);
    emit sendNewProfile(gender, nick, name);

    if (m_network) {
      emit sendSyncProfile(gender, nick, "", name);
      if (m_remoteNumeric && local)
        m_link->sendSyncProfile(gender, nick, "", name);
    }
  }
  else if (!m_users.contains(lowerNewNick) || lowerNick == lowerNewNick) {
    if (m_channelLog) {
      if (gender)
        m_channelLog->msg(tr("\"%1\" is now known as \"%2\"", "Female").arg(nick).arg(nNick));
      else
        m_channelLog->msg(tr("\"%1\" is now known as \"%2\"", "Male").arg(nick).arg(nNick));
    }

    UserUnit *unit = m_users.value(lowerNick);
    if (m_network) {
      if (unit->numeric() == m_numeric && !local)
        return;
    }
    m_users.remove(lowerNick);
    m_users.insert(lowerNewNick, unit);
    unit->profile()->setGender(gender);
    unit->profile()->setNick(nNick);
    unit->profile()->setFullName(name);

    emit sendNewNick(gender, nick, nNick, name);

    if (m_network) {
      emit sendSyncProfile(gender, nick, nNick, name);
      if (m_remoteNumeric && local) {
        m_link->sendSyncProfile(gender, nick, nNick, name);
      }
    }
  }
  else if (local && m_users.contains(lowerNewNick)) {
    DaemonService *service = qobject_cast<DaemonService *>(sender());
    if (service)
      service->quit();
  }
}


/*!
 * Обновляет статус пользователей, исключая локальных.
 */
void Daemon::updateStatus(quint32 status, const QStringList &users)
{
  foreach (QString user, users) {
    QString lowerNick = normalizeNick(user);
    if (m_users.contains(lowerNick)) {
      UserUnit *unit = m_users.value(lowerNick);
      if (unit->numeric() != m_numeric)
        unit->profile()->setStatus(status);
    }
  }
}
