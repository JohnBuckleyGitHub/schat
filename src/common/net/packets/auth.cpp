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
#include "net/SimpleID.h"

AuthReplyData::AuthReplyData(const QByteArray &serverId, const QByteArray &userId, const QByteArray &session)
  : serverId(serverId)
  , session(session)
  , userId(userId)
  , error(NoError)
  , status(AccessGranted)
{
}


AuthReplyData::AuthReplyData(const QByteArray &serverId, int error)
  : serverId(serverId)
  , error(error)
  , status(AccessDenied)
{
}


AuthReplyWriter::AuthReplyWriter(QDataStream *stream, const AuthReplyData &data)
  : PacketWriter(stream, Protocol::AuthReplyPacket, data.serverId, data.userId)
{
  put(data.status);

  if (data.status == AuthReplyData::AccessGranted) {
    putId(data.session);
  }
  else {
    put(data.error);
  }
}


AuthReplyReader::AuthReplyReader(PacketReader *reader)
{
  data.serverId = reader->sender();
  data.userId = reader->dest();
  data.error = 0;
  data.status = reader->get<quint8>();

  if (data.status == AuthReplyData::AccessGranted) {
    data.session = reader->id();
  }
  else {
    data.error = reader->get<quint8>();
  }
}


AuthRequestData::AuthRequestData(int authType, const QString &host, const QString &nick)
  : host(host)
  , nick(nick)
  , features(SupportRichText)
  , authType(authType)
  , authVersion(V1)
  , maxProtoVersion(0x0)
{
  userAgent = genUserAgent();
}


/*!
 * Получение строки UserAgent.
 */
QString AuthRequestData::genUserAgent()
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


AuthRequestWriter::AuthRequestWriter(QDataStream *stream, const AuthRequestData &data)
  : PacketWriter(stream, Protocol::AuthRequestPacket)
{
  put(data.authVersion);
  put(data.authType);
  putId(SimpleID::uniqueId());
  put(data.maxProtoVersion);
  put(data.features);
  put(data.language);
  put(data.host);
  put(data.nick);
  put(data.userAgent);
}


AuthRequestReader::AuthRequestReader(PacketReader *reader)
{
  data.authVersion = reader->get<quint8>();
  data.authType = reader->get<quint8>();
  data.uniqueId = reader->id();
  data.maxProtoVersion = reader->get<quint8>();
  data.features = reader->get<quint32>();
  data.language = reader->get<quint8>();
  data.host = reader->text();
  data.nick = reader->text();
  data.userAgent = reader->text();
}
