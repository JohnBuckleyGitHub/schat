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
#include "SettingsHook.h"
#include "sglobal.h"
#include "Storage.h"
#include "StorageHook.h"

QStringList Storage::m_features;
Storage *Storage::m_self = 0;


Storage::Storage(const QString &app, QObject *parent)
  : QObject(parent)
{
  m_self = this;
  qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()) ^ reinterpret_cast<quintptr>(this));

  Path::init(app);
  Normalize::init();

  new DataBase(this);
  new Ch(this);

  // Инициализация настроек по умолчанию.
  m_settings = new Settings(etcPath() + LC('/') + Path::app() + LS(".conf"), this);
  m_settings->setDefault(LS("AnonymousAuth"), true);
  m_settings->setDefault(LS("AuthServer"),    LS("https://auth.schat.me"));
  m_settings->setDefault(LS("Certificate"),   LS("server.crt"));
  m_settings->setDefault(LS("Listen"),        QStringList("0.0.0.0:7667"));
  m_settings->setDefault(LS("LogLevel"),      2);
  m_settings->setDefault(LS("MaxOpenFiles"),  0);
  m_settings->setDefault(LS("NickOverride"),  true);
  m_settings->setDefault(LS("PrivateId"),     QString(SimpleID::encode(SimpleID::uniqueId())));
  m_settings->setDefault(LS("PrivateKey"),    LS("server.key"));
  m_settings->setDefault(LS("Workers"),       0);

  m_log = new NodeLog();
  new FeedStorage(this);

  add(new SettingsHook());
}


Storage::~Storage()
{
  qDeleteAll(m_hooks);
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


void Storage::addFeature(const QString &name)
{
  if (!m_features.contains(name))
    m_features.append(name);
}


bool Storage::contains(const QString &key)
{
  if (m_self->m_cache.contains(key) || m_self->m_keys.contains(key) || DataBase::contains(key))
    return true;

  return false;
}

/*!
 * Запись данных в хранилище.
 *
 * \param key   Ключ в хранилище.
 * \param value Данные для записи в хранилище.
 */
int Storage::setValue(const QString &key, const QVariant &value)
{
  if (m_self->m_cache.value(key) == value)
    return Notice::NotModified;

  bool cache = true;
  StorageHook *hook = m_self->m_keys.value(key);
  if (hook) {
    if (!hook->setValue(key, value))
      return Notice::Forbidden;

    cache = hook->cache();
  }
  else
    DataBase::setValue(key, value);

  if (cache) {
    m_self->m_cache[key] = value;
    emit m_self->valueChanged(key, value);
  }

  return Notice::OK;
}


/*!
 * Получение данных из хранилища.
 *
 * \param key          Ключ в хранилище.
 * \param defaultValue Значение по умолчанию.
 */
QVariant Storage::value(const QString &key, const QVariant &defaultValue)
{
  if (m_self->m_cache.contains(key))
    return m_self->m_cache.value(key);

  bool cache = true;
  StorageHook *hook = m_self->m_keys.value(key);
  QVariant value;

  if (hook) {
    value = hook->value(key, defaultValue);
    cache = hook->cache();
  }
  else
    value = DataBase::value(key, defaultValue);

  if (cache)
    m_self->m_cache[key] = value;

  return value;
}


/*!
 * Добавление хука.
 */
void Storage::add(StorageHook *hook)
{
  if (!hook)
    return;

  QStringList keys = hook->keys();
  if (keys.isEmpty()) {
    delete hook;
    return;
  }

  m_self->m_hooks.append(hook);
  foreach (const QString &key, keys) {
    m_self->m_keys[key] = hook;
  }
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

# if defined(Q_OS_UNIX)
  setMaxOpenFiles(m_settings->value(LS("MaxOpenFiles")).toInt());
# endif

  m_privateId = m_settings->value(LS("PrivateId")).toString().toUtf8();
  if (m_privateId == SimpleID::encode(SimpleID::uniqueId())) {
    SCHAT_LOG_WARN("Сonfiguration option \"PrivateId\" uses a default value, please set your own private ID")
  }

  m_id = SimpleID::make(m_privateId, SimpleID::ServerId);

  DataBase::start();
  return 0;
}


void Storage::setDefaultSslConf()
{
# if !defined(SCHAT_NO_SSL)
  if (!QSslSocket::supportsSsl())
    return;

  QList<QSslCertificate> certificates = QSslCertificate::fromPath(Path::file(etcPath(), m_settings->value(LS("Certificate")).toString()));
  if (certificates.isEmpty())
    return;

  QSslConfiguration conf = QSslConfiguration::defaultConfiguration();

  QFile key(Path::file(etcPath(), m_settings->value(LS("PrivateKey")).toString()));
  if (key.exists() && key.open(QFile::ReadOnly)) {
    conf.setPrivateKey(QSslKey(&key, QSsl::Rsa));
    key.close();
  }

  conf.setLocalCertificate(certificates.takeFirst());

  if (!certificates.isEmpty()) {
    QList<QSslCertificate> ca = conf.caCertificates();
    foreach (const QSslCertificate &cert, certificates) {
      ca.append(cert);
    }

    conf.setCaCertificates(ca);
  }

  conf.setProtocol(QSsl::TlsV1);
  QSslConfiguration::setDefaultConfiguration(conf);
# endif
}


void Storage::setMaxOpenFiles(int max)
{
# if defined(Q_OS_UNIX)
  struct rlimit limit;

  if (max > 0) {
    limit.rlim_cur = max;
    limit.rlim_max = max;

    setrlimit(RLIMIT_NOFILE, &limit);
  }

  if (getrlimit(RLIMIT_NOFILE, &limit) == 0) {
    SCHAT_LOG_INFO("Max open files limit:" << limit.rlim_cur << limit.rlim_max);
  }
# else
  Q_UNUSED(max)
# endif
}
