/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QNetworkInterface>

#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "net/PacketWriter.h"
#include "net/Protocol.h"

#define SCHAT_RANDOM_CLIENT_ID
#include <QUuid>


AuthReply::AuthReply(const QByteArray &serverId, const QByteArray &clientId, const QByteArray &session)
  : Packet(Protocol::AuthReplyPacket)
  , m_serverId(serverId)
  , m_session(session)
  , m_userId(clientId)
  , m_error(0)
  , m_status(AccessGranted)
{
}


AuthReply::AuthReply(Error error)
  : Packet(Protocol::AuthReplyPacket)
  , m_error(error)
  , m_status(AccessDenied)
{
}


AuthReply::AuthReply(PacketReader *reader)
  : Packet(Protocol::AuthReplyPacket)
  , m_error(0)
{
  m_status = reader->get<quint8>();
  if (m_status == AccessGranted) {
    m_serverId = reader->id();
    m_userId = reader->id();
    m_session = reader->id();
  }
  else {
    m_error = reader->get<quint8>();
  }
}


AuthReply::~AuthReply()
{
}


void AuthReply::body()
{
  m_writer->put(m_status);
  if (m_status == AccessGranted) {
    m_writer->putId(m_serverId);
    m_writer->putId(m_userId);
    m_writer->putId(m_session);
  }
  else {
    m_writer->put(m_error);
  }
}




AuthRequest::AuthRequest(PacketReader *reader)
  : Packet(Protocol::AuthRequestPacket)
{
  m_authVersion = reader->get<quint8>();
  m_authType = reader->get<quint8>();
  m_uniqueId = reader->id();
  m_maxProtoVersion = reader->get<quint8>();
  m_features = reader->get<quint32>();
  m_language = reader->get<quint8>();
  m_host = reader->text();
  m_nick = reader->text();
  m_userAgent = reader->text();
}


AuthRequest::AuthRequest(AuthType authType, const QString &host, const QString &nick)
  : Packet(Protocol::AuthRequestPacket)
  , m_reader(0)
  , m_host(host)
  , m_nick(nick)
  , m_features(SupportRichText)
  , m_authType(authType)
  , m_authVersion(V1)
  , m_maxProtoVersion(0x0)
{
  m_userAgent = genUserAgent();
}


AuthRequest::~AuthRequest()
{
}


bool AuthRequest::isValid() const
{
  return true;
}


/*!
 * Получение уникального идентификатора клиента на основе
 * mac адреса первого активного сетевого интерфейса.
 */
QByteArray AuthRequest::genUniqueId()
{
  #if !defined(SCHAT_DAEMON) && defined(SCHAT_RANDOM_CLIENT_ID)
  return QCryptographicHash::hash(QUuid::createUuid().toString().toLatin1(), QCryptographicHash::Sha1);
  #endif

  QList<QNetworkInterface> all = QNetworkInterface::allInterfaces();
  foreach (QNetworkInterface iface, all) {
    QString hw = iface.hardwareAddress();
    if (!hw.isEmpty() && iface.flags().testFlag(QNetworkInterface::IsUp) && iface.flags().testFlag(QNetworkInterface::IsRunning)) {
      return QCryptographicHash::hash(hw.toLatin1(), QCryptographicHash::Sha1);
    }
  }

  return QCryptographicHash::hash("", QCryptographicHash::Sha1);
}


QString AuthRequest::genUserAgent()
{
  #if defined(SCHAT_DAEMON)
  QString out = "ISCd/";
  #else
  QString out = "ISC/";
  #endif

  out += QCoreApplication::applicationVersion();
  #if defined(Q_OS_FREEBSD)
  out += "/FreeBSD";
  #elif defined(Q_OS_LINUX)
  out += "/Linux";
  #elif defined(Q_OS_MAC)
  out += "/MacOSX";
  #elif defined(Q_OS_NETBSD)
  out += "/NetBSD";
  #elif defined(Q_OS_OPENBSD)
  out += "/OpenBSD";
  #elif defined(Q_OS_WIN32)
  out += "/Windows";
  #endif

  if (QSysInfo::WordSize == 64)
    out += "/64bit";

  return out;
}


void AuthRequest::body()
{
  m_writer->put(m_authVersion);
  m_writer->put(m_authType);
  m_writer->putId(genUniqueId() += Protocol::UniqueUserId);
  m_writer->put(m_maxProtoVersion);
  m_writer->put(m_features);
  m_writer->put(m_language);
  m_writer->put(m_host);
  m_writer->put(m_nick);
  m_writer->put(m_userAgent);
}
