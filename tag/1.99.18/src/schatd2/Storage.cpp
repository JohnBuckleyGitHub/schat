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
#include <QUuid>

#if defined(Q_OS_UNIX)
#include <sys/resource.h>
#endif

#include "Ch.h"
#include "DataBase.h"
#include "DateTime.h"
#include "debugstream.h"
#include "feeds/FeedStorage.h"
#include "FileLocations.h"
#include "net/packets/auth.h"
#include "net/Protocol.h"
#include "net/SimpleID.h"
#include "NodeLog.h"
#include "Normalize.h"
#include "Settings.h"
#include "Storage.h"

Storage *Storage::m_self = 0;

Storage::Storage(QObject *parent)
  : QObject(parent)
{
  m_self = this;

  Normalize::init();

  new Ch(this);

  m_locations = new FileLocations(this);

  // Инициализация настроек по умолчанию.
  m_settings = new Settings(m_locations->path(FileLocations::ConfigFile), this);
  m_settings->setDefault("Certificate",  QLatin1String("server.crt"));
  m_settings->setDefault("Listen",       QStringList("0.0.0.0:7667"));
  m_settings->setDefault("LogLevel",     2);
  m_settings->setDefault("MaxOpenFiles", 0);
  m_settings->setDefault("PrivateId",    QString(SimpleID::encode(SimpleID::uniqueId())));
  m_settings->setDefault("PrivateKey",   QLatin1String("server.key"));
  m_settings->setDefault("Workers",      0);

  m_log = new NodeLog;
  new FeedStorage(this);
}


Storage::~Storage()
{
  delete m_log;
}


QString Storage::serverName()
{
  qDebug() << "serverName()" << Ch::server()->name();

  return Ch::server()->name();
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
  m_log->open(m_locations->file(FileLocations::LogPath, m_locations->path(FileLocations::BaseName) + ".log"), static_cast<NodeLog::Level>(m_settings->value("LogLevel").toInt()));

  setDefaultSslConf();
  setMaxOpenFiles(m_settings->value("MaxOpenFiles").toInt());

  m_privateId = m_settings->value("PrivateId").toString().toUtf8();
  m_id = SimpleID::make(m_privateId, SimpleID::ServerId);

  DataBase::start();
  return 0;
}


void Storage::setDefaultSslConf()
{
# if !defined(SCHAT_NO_SSL)
  if (!QSslSocket::supportsSsl())
    return;

  QString crtFile = m_locations->file(FileLocations::ConfigPath, m_settings->value(QLatin1String("Certificate")).toString());
  if (crtFile.isEmpty())
    return;

  QString keyFile = m_locations->file(FileLocations::ConfigPath, m_settings->value(QLatin1String("PrivateKey")).toString());
  if (crtFile.isEmpty())
    return;

  QSslConfiguration conf = QSslConfiguration::defaultConfiguration();

  QFile file(crtFile);
  if (file.open(QIODevice::ReadOnly)) {
    conf.setLocalCertificate(QSslCertificate(&file));
    file.close();
  }
  else {
    SCHAT_LOG_WARN() << "Could not open Certificate file" << file.fileName() << ":" << file.errorString();
  }

  file.setFileName(keyFile);
  if (file.open(QIODevice::ReadOnly)) {
    conf.setPrivateKey(QSslKey(&file, QSsl::Rsa));
    file.close();
  }
  else {
    SCHAT_LOG_WARN() << "Could not open Private Key file" << file.fileName() << ":" << file.errorString();
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
