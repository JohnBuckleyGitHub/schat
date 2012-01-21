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

#include <QCoreApplication>
#include <QNetworkInterface>

#include "Account.h"
#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "net/PacketWriter.h"
#include "net/Protocol.h"
#include "net/SimpleID.h"
#include "Channel.h"

AuthReply::AuthReply(PacketReader *reader)
{
  serverId = reader->sender();
  userId = reader->dest();

  fields = reader->get<quint8>();
  status = reader->get<quint16>();
  id = reader->id();

  if (status == Notice::OK) {
    cookie = reader->id();
    serverName = reader->text();
    account = reader->text();
  }

  if (fields & JSonField)
    json = reader->json();
}


QByteArray AuthReply::data(QDataStream *stream) const
{
  if (!json.isNull())
    fields |= JSonField;

  PacketWriter writer(stream, Protocol::AuthReplyPacket, serverId, userId);
  writer.put(fields);
  writer.put(status);
  writer.putId(id);

  if (status == Notice::OK) {
    writer.putId(cookie);
    writer.put<quint32>(0);
    writer.put<quint8>(0);
    writer.put(serverName);
    writer.put(account);
  }

  if (fields & JSonField)
    writer.put(json);

  return writer.data();
}


AuthRequest::AuthRequest(int authType, const QString &host, Channel *channel, const QVariant &json)
  : fields(0)
  , authType(authType)
  , gender(channel->gender().raw())
  , host(host)
  , nick(channel->name())
  , userAgent(SimpleID::userAgent())
  , json(json)
{
  setStatus(channel->status().value());
}


AuthRequest::AuthRequest(PacketReader *reader)
{
  fields = reader->get<quint8>();
  authType = reader->get<quint8>();
  uniqueId = reader->id();
  id = reader->id();
  gender = reader->get<quint8>();
  setStatus(reader->get<quint8>());
  host = reader->text();
  nick = reader->text();
  userAgent = reader->text();

  if (authType == SlaveNode)
    privateId = reader->text();

  if (authType == Password) {
    account = reader->text();
    password = reader->id();
    cookie = reader->id();
  }

  if (authType == Cookie)
    cookie = reader->id();

  if (fields & JSonField)
    json = reader->json();
}


bool AuthRequest::isValid() const
{
  if (SimpleID::typeOf(uniqueId) != SimpleID::UniqueUserId)
    return false;

  if (SimpleID::typeOf(id) != SimpleID::MessageId)
    return false;

  if (userAgent.isEmpty())
    return false;

  if (!Channel::isValidName(nick))
    return false;

  if (authType == Cookie && SimpleID::typeOf(cookie) != SimpleID::CookieId)
    return false;

  if (fields & JSonField && json.isNull())
    return false;

  return true;
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
  writer.put(gender);
  writer.put(status);
  writer.put(host);
  writer.put(nick);
  writer.put(userAgent);

  if (authType == SlaveNode)
    writer.put(privateId);

  if (authType == Password) {
    writer.put(account);
    writer.putId(password);
    writer.putId(cookie);
  }

  if (authType == Cookie)
    writer.putId(cookie);

  if (fields & JSonField)
    writer.put(json);

  return writer.data();
}


void AuthRequest::setStatus(quint8 status)
{
  if (status == Status::Offline)
    status = Status::Online;

  this->status = status;
}
