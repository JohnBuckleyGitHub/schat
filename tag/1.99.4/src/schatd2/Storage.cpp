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

#include "Ch.h"
#include "DataBase.h"
#include "DateTime.h"
#include "debugstream.h"
#include "feeds/FeedStorage.h"
#include "FileLocations.h"
#include "net/packets/auth.h"
#include "net/Protocol.h"
#include "net/ServerData.h"
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

  m_serverData = new ServerData();
  m_locations = new FileLocations(this);

  // Инициализация настроек по умолчанию.
  m_settings = new Settings(m_locations->path(FileLocations::ConfigFile), this);
  m_settings->setDefault("Certificate", QLatin1String("server.crt"));
  m_settings->setDefault("Kernel",      QString());
  m_settings->setDefault("Listen",      QStringList("0.0.0.0:7667"));
  m_settings->setDefault("LogLevel",    2);
  m_settings->setDefault("MainChannel", 2);
  m_settings->setDefault("PrivateId",   QString(SimpleID::encode(SimpleID::uniqueId())));
  m_settings->setDefault("PrivateKey",  QLatin1String("server.key"));
  m_settings->setDefault("ServerName",  QString());

  m_log = new NodeLog;
  new FeedStorage(this);
}


Storage::~Storage()
{
  delete m_log;
  delete m_serverData;
}


int Storage::load()
{
  ChatChannel server = Ch::channel(m_serverData->id(), SimpleID::ServerId);
  qDebug() << " - - - - - - - ";
  qDebug() << " - - - - - - - " << server;
  qDebug() << " - - - - - - - ";
  if (!server) {
    server = ChatChannel(new ServerChannel(m_serverData->id(), m_serverData->name()));
  }

  server->setName(m_serverData->name());
  Ch::add(server);

  Ch::channel(QString("Main"));

  qint64 key = m_settings->value("MainChannel").toLongLong();
  if (key > 0) {
    ChatChannel channel = DataBase::channel(key);
    if (channel) {
      m_serverData->setChannelId(channel->id());
    }
  }

  return 0;
}


/*!
 * Запуск сервера, функция производит инициализацию состояния и объектов сервера.
 */
int Storage::start()
{
  m_log->open(m_locations->file(FileLocations::LogPath, m_locations->path(FileLocations::BaseName) + ".log"), static_cast<NodeLog::Level>(m_settings->value("LogLevel").toInt()));

  setDefaultSslConf();

  m_serverData->setPrivateId(m_settings->value("PrivateId").toString().toUtf8());
  m_serverData->setName(m_settings->value("ServerName").toString());

  DataBase::start();
  return 0;
}


/*!
 * Создание идентификатора пользователя.
 */
QByteArray Storage::makeUserId(int type, const QByteArray &userId) const
{
  QByteArray prefix;
  if (type == AuthRequest::Anonymous || type == AuthRequest::Cookie || type == AuthRequest::Password)
    prefix = "anonymous:";
  else if (type == AuthRequest::SlaveNode)
    prefix = "slave:";

  return SimpleID::make(prefix + m_serverData->privateId() + userId, SimpleID::UserId);
}


/*!
 * Генерирование новой Cookie.
 */
QByteArray Storage::cookie() const
{
  return SimpleID::randomId(SimpleID::CookieId, m_serverData->privateId());
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