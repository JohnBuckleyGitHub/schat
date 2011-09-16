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
#include "User.h"

AuthReplyData::AuthReplyData(ServerData *data, int error)
  : status(AccessDenied)
  , protoVersion(Protocol::V4_0)
  , error(error)
{
  serverData.setId(data->id());
}


AuthReplyData::AuthReplyData(ServerData *data, User *user, const QByteArray &cookie)
  : userId(user->id())
  , status(AccessGranted)
  , cookie(cookie)
  , protoVersion(Protocol::V4_0)
  , error(NoError)
{
  serverData.setId(data->id());
  serverData.setName(data->name());
  serverData.setChannelId(data->channelId());
  serverData.setFeatures(data->features());
  serverData.setNumber(user->serverNumber());
}


AuthReplyWriter::AuthReplyWriter(QDataStream *stream, const AuthReplyData &data)
  : PacketWriter(stream, Protocol::AuthReplyPacket, data.serverData.id(), data.userId)
{
  put(data.status);
  put(data.serverData.number());

  if (data.status == AuthReplyData::AccessGranted) {
    putId(data.cookie);
    put(data.protoVersion);
    put<quint16>(0);
    put<quint16>(0);
    put(data.serverData.features());

    if (data.serverData.is(ServerData::AutoJoinSupport))
      putId(data.serverData.channelId());

    put(data.serverData.name());
  }
  else {
    put(data.error);
  }
}


AuthReplyReader::AuthReplyReader(PacketReader *reader)
{
  data.serverData.setId(reader->sender());
  data.userId = reader->dest();
  data.error = 0;
  data.status = reader->get<quint8>();
  data.serverData.setNumber(reader->get<quint8>());

  if (data.status == AuthReplyData::AccessGranted) {
    data.cookie = reader->id();
    data.protoVersion = reader->get<quint8>();
    reader->get<quint16>();
    reader->get<quint16>();
    data.serverData.setFeatures(reader->get<quint32>());

    if (data.serverData.is(ServerData::AutoJoinSupport))
      data.serverData.setChannelId(reader->id());

    data.serverData.setName(reader->text());
  }
  else {
    data.error = reader->get<quint8>();
  }
}


AuthRequestData::AuthRequestData(int authType, const QString &host, User *user)
  : fields(0)
  , authType(authType)
  , gender(user->rawGender())
  , host(host)
  , nick(user->nick())
  , userAgent(SimpleID::userAgent())
{
  setStatus(user->status());
}


void AuthRequestData::setStatus(quint8 status)
{
  if (status == User::OfflineStatus)
    status = User::OnlineStatus;

  this->status = status;
}


AuthRequestWriter::AuthRequestWriter(QDataStream *stream, const AuthRequestData &data)
  : PacketWriter(stream, Protocol::AuthRequestPacket)
{
  put(data.fields);
  put(data.authType);
  putId(data.uniqueId);
  put<quint8>(0);
  put<quint32>(0);
  put<quint8>(0);
  put(data.gender);
  put(data.status);
  put(data.host);
  put(data.nick);
  put(data.userAgent);

  if (data.authType == AuthRequestData::SlaveNode)
    put(data.privateId);

  if (data.authType == AuthRequestData::Cookie) {
    putId(data.cookie);
  }
}


AuthRequestReader::AuthRequestReader(PacketReader *reader)
{
  data.fields = reader->get<quint8>();
  data.authType = reader->get<quint8>();
  data.uniqueId = reader->id();
  reader->get<quint8>();
  reader->get<quint32>();
  reader->get<quint8>();
  data.gender = reader->get<quint8>();
  data.setStatus(reader->get<quint8>());
  data.host = reader->text();
  data.nick = reader->text();
  data.userAgent = reader->text();

  if (data.authType == AuthRequestData::SlaveNode)
    data.privateId = reader->text();

  if (data.authType == AuthRequestData::Cookie)
    data.cookie = reader->id();
}
