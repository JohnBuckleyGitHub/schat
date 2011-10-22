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

AuthReply::AuthReply(PacketReader *reader)
  : AbstractPacket(reader)
{
  serverData.setId(reader->sender());
  userId = reader->dest();

  fields = reader->get<quint8>();
  status = reader->get<quint16>();
  id = reader->id();

  if (status == Notice::OK) {
    cookie = reader->id();
    serverData.setFeatures(reader->get<quint32>());
    serverData.setNumber(reader->get<quint8>());
    serverData.setName(reader->text());
  }

  if (fields & JSonField)
    json = reader->json();

  if (status == Notice::OK) {
    if (serverData.is(ServerData::AutoJoinSupport))
      serverData.setChannelId(reader->id());
  }
}


AuthReply::AuthReply(ServerData *data, int status, const QByteArray &id, const QVariant &json)
  : AbstractPacket(json)
  , status(status)
  , id(id)
{
  serverData.setId(data->id());
  serverData.setName(data->name());
}


AuthReply::AuthReply(ServerData *data, User *user, const QByteArray &cookie, const QByteArray &id, const QVariant &json)
  : AbstractPacket(json)
  , userId(user->id())
  , status(Notice::OK)
  , cookie(cookie)
  , id(id)
{
  serverData.setId(data->id());
  serverData.setName(data->name());
  serverData.setChannelId(data->channelId());
  serverData.setFeatures(data->features());
  serverData.setNumber(user->serverNumber());
}


QByteArray AuthReply::data(QDataStream *stream) const
{
  if (!json.isNull())
    fields |= JSonField;

  PacketWriter writer(stream, Protocol::AuthReplyPacket, serverData.id(), userId);
  writer.put(fields);
  writer.put(status);
  writer.putId(id);

  if (status == Notice::OK) {
    writer.putId(cookie);
    writer.put(serverData.features());
    writer.put(serverData.number());
    writer.put(serverData.name());
  }

  if (fields & JSonField)
    writer.put(json);

  if (status == Notice::OK) {
    if (serverData.is(ServerData::AutoJoinSupport))
      writer.putId(serverData.channelId());
  }

  return writer.data();
}


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


AuthRequest::AuthRequest(int authType, const QString &host, User *user, const QVariant &json)
  : AbstractPacket(json)
  , authType(authType)
  , gender(user->rawGender())
  , host(host)
  , nick(user->nick())
  , userAgent(SimpleID::userAgent())
{
  setStatus(user->status());
}


AuthRequest::AuthRequest(PacketReader *reader)
  : AbstractPacket(reader)
{
  fields = reader->get<quint8>();
  authType = reader->get<quint8>();
  uniqueId = reader->id();
  id = reader->id();
  reader->get<quint8>();
  reader->get<quint32>();
  reader->get<quint8>();
  gender = reader->get<quint8>();
  setStatus(reader->get<quint8>());
  host = reader->text();
  nick = reader->text();
  userAgent = reader->text();

  if (authType == SlaveNode)
    privateId = reader->text();

  if (authType == Cookie)
    cookie = reader->id();

  if (fields & JSonField)
    json = reader->json();
}


QByteArray AuthRequest::data(QDataStream *stream) const
{
  if (!json.isNull())
    fields |= JSonField;

  if (id.isEmpty())
    id = SimpleID::randomId(SimpleID::MessageId);

  PacketWriter writer(stream, Protocol::AuthRequestPacket);

  writer.put(fields);
  writer.put(authType);
  writer.putId(uniqueId);
  writer.putId(id);
  writer.put<quint8>(0);
  writer.put<quint32>(0);
  writer.put<quint8>(0);
  writer.put(gender);
  writer.put(status);
  writer.put(host);
  writer.put(nick);
  writer.put(userAgent);

  if (authType == SlaveNode)
    writer.put(privateId);

  if (authType == Cookie)
    writer.putId(cookie);

  if (fields & JSonField)
    writer.put(json);

  return writer.data();
}


void AuthRequest::setStatus(quint8 status)
{
  if (status == User::OfflineStatus)
    status = User::OnlineStatus;

  this->status = status;
}
