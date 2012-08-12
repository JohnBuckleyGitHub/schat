/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

#include <QFile>
#include <QSslConfiguration>
#include <QSslKey>
#include <QStringList>
#include <QTime>
#include <QUuid>

#if defined(Q_OS_UNIX)
#include <sys/resource.h>
#endif

#include "Ch.h"
#include "DataBase.h"
#include "DateTime.h"
#include "debugstream.h"
#include "feeds/FeedStorage.h"
#include "net/packets/auth.h"
#include "net/Protocol.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "Normalize.h"
#include "Path.h"
#include "Settings.h"
#include "sglobal.h"
#include "Storage.h"

QStringList Storage::m_features;
Storage *Storage::m_self = 0;


Storage::Storage(QObject *parent)
  : QObject(parent)
  , m_anonymous(true)
  , m_nickOverride(true)
{
  m_self = this;
  qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()) ^ reinterpret_cast<quintptr>(this));

  Path::init();
  Normalize::init();

  new DataBase(this);
  new Ch(this);

  // Инициализация настроек по умолчанию.
  m_settings = new Settings(etcPath() + LC('/') + Path::app() + LS(".conf"), this);
  m_settings->setDefault(LS("AnonymousAuth"), m_anonymous);
  m_settings->setDefault(LS("AuthServer"),    LS("https://auth.schat.me"));
  m_settings->setDefault(LS("Certificate"),   LS("server.crt"));
  m_settings->setDefault(LS("Listen"),        QStringList("0.0.0.0:7667"));
  m_settings->setDefault(LS("LogLevel"),      2);
  m_settings->setDefault(LS("MaxOpenFiles"),  0);
  m_settings->setDefault(LS("NickOverride"),  m_nickOverride);
  m_settings->setDefault(LS("PrivateId"),     QString(SimpleID::encode(SimpleID::uniqueId())));
  m_settings->setDefault(LS("PrivateKey"),    LS("server.key"));
  m_settings->setDefault(LS("Workers"),       0);

  m_log = new NodeLog();
  new FeedStorage(this);
}


Storage::~Storage()
{
  delete m_log;
}


bool Storage::hasFeature(const QString &name)
{
  return m_features.contains(name);
}


QString Storage::etcPath()
{
# if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
  if (!Path::isPortable())
    return LS("/etc/schatd2");
# endif

  return Path::data();
}


QString Storage::serverName()
{
  qDebug() << "serverName()" << Ch::server()->name();

  return Ch::server()->name();
}


QString Storage::sharePath()
{
# if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
  if (!Path::isPortable())
    return LS("/usr/share/schatd2");
# endif

  return Path::data(Path::SystemScope);
}


QString Storage::varPath()
{
# if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
  if (!Path::isPortable())
    return LS("/var/lib/schatd2");
# endif

  return Path::cache();
}



/*!
 * Получение данных из хранилища.
 *
 * \param key          Ключ в хранилище.
 * \param defaultValue Значение по умолчанию.
 */
QVariant Storage::value(const QString &key, const QVariant &defaultValue)
{
  return DataBase::value(key, defaultValue);
}


void Storage::addFeature(const QString &name)
{
  if (!m_features.contains(name))
    m_features.append(name);
}


/*!
 * Запись данных в хранилище.
 *
 * \param key   Ключ в хранилище.
 * \param value Данные для записи в хранилище.
 */
void Storage::setValue(const QString &key, const QVariant &value)
{
  DataBase::setValue(key, value);
}


int Storage::load()
{
  Ch::load();

  return 0;
}


/*!
 * Запуск сервера, функция производит инициализацию состояния и объектов сервера.
 */
int Storage::start()
{
  QString logPath = Path::cache();
# if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
  if (!Path::isPortable())
    logPath = LS("/var/log/") + Path::app();
# endif

  m_log->open(logPath + LC('/') + Path::app() + LS(".log"), static_cast<NodeLog::Level>(m_settings->value(LS("LogLevel")).toInt()));

  setDefaultSslConf();
  setMaxOpenFiles(m_settings->value(LS("MaxOpenFiles")).toInt());

  m_privateId = m_settings->value(LS("PrivateId")).toString().toUtf8();
  if (m_privateId == SimpleID::encode(SimpleID::uniqueId())) {
    SCHAT_LOG_WARN("Сonfiguration option \"PrivateId\" uses a default value, please set your own private ID")
  }

  m_id           = SimpleID::make(m_privateId, SimpleID::ServerId);
  m_anonymous    = m_settings->value(LS("AnonymousAuth")).toBool();
  m_nickOverride = m_settings->value(LS("NickOverride")).toBool();
  m_authServer   = m_settings->value(LS("AuthServer")).toString();

  DataBase::start();
  return 0;
}


void Storage::setDefaultSslConf()
{
# if !defined(SCHAT_NO_SSL)
  if (!QSslSocket::supportsSsl())
    return;

  QString crtFile = Path::file(etcPath(), m_settings->value(LS("Certificate")).toString());
  if (crtFile.isEmpty())
    return;

  QString keyFile = Path::file(etcPath(), m_settings->value(LS("PrivateKey")).toString());
  if (crtFile.isEmpty())
    return;

  QSslConfiguration conf = QSslConfiguration::defaultConfiguration();

  QFile file(crtFile);
  if (file.open(QIODevice::ReadOnly)) {
    conf.setLocalCertificate(QSslCertificate(&file));
    file.close();
  }
  else {
    SCHAT_LOG_WARN("Could not open Certificate file" << file.fileName() << ":" << file.errorString())
  }

  file.setFileName(keyFile);
  if (file.open(QIODevice::ReadOnly)) {
    conf.setPrivateKey(QSslKey(&file, QSsl::Rsa));
    file.close();
  }
  else {
    SCHAT_LOG_WARN("Could not open Private Key file" << file.fileName() << ":" << file.errorString())
  }

  QSslConfiguration::setDefaultConfiguration(conf);
# endif
}


void Storage::setMaxOpenFiles(int max)
{
  if (max <= 0)
    return;

# if defined(Q_OS_UNIX)
  struct rlimit limit;
  limit.rlim_cur = max;
  limit.rlim_max = max;

  qDebug() << setrlimit(RLIMIT_NOFILE, &limit);
# endif
}
