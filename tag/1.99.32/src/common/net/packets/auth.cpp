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

#include <QNetworkInterface>
#include <QHostInfo>

#include "Account.h"
#include "Channel.h"
#include "DateTime.h"
#include "net/PacketReader.h"
#include "net/packets/auth.h"
#include "net/PacketWriter.h"
#include "net/Protocol.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "tools/OsInfo.h"
#include "tools/Ver.h"

AuthReply::AuthReply(PacketReader *reader)
{
  serverId = reader->sender();
  userId = reader->dest();

  fields = reader->get<quint8>();
  status = reader->get<quint16>();
  id = reader->id();

  if (status == Notice::OK) {
    cookie = reader->id();
    reader->get<quint32>();
    reader->get<quint8>();
    serverName = reader->text();
    account = reader->text();
  }

  if (fields & JSonField)
    json = reader->json().toMap();

  if (fields & HostField) {
    host = reader->text();
    hostId = reader->id();
  }
}


QByteArray AuthReply::data(QDataStream *stream) const
{
  if (!json.isEmpty())
    fields |= JSonField;

  if (!host.isEmpty())
    fields |= HostField;

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

  if (fields & HostField) {
    writer.put(host);
    writer.putId(hostId);
  }

  return writer.data();
}


AuthRequest::AuthRequest(int authType, const QString &host, Channel *channel)
  : fields(ExtraInfoField)
  , authType(authType)
  , gender(channel->gender().raw())
  , host(host)
  , nick(channel->name())
  , userAgent(LC('i'))
{
  json = OsInfo::json();
  setStatus(channel->status().value());
}


AuthRequest::AuthRequest(PacketReader *reader)
  : os(0)
  , version(0)
  , tz(DateTime::tz())
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

  if (fields & JSonField) {
    raw = reader->get<QByteArray>();

    if (raw.size() <= MaxJSONSize)
      json = JSON::parse(raw).toMap();
  }

  if (fields & ExtraInfoField) {
    os       = reader->get<quint8>();
    version  = reader->get<quint32>();
    tz       = reader->get<qint32>();
    hostName = reader->text();
  }
}


/*!
 * Проверка пакета на корректность.
 */
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

  if (fields & JSonField && (raw.isEmpty() || raw.size() > MaxJSONSize))
    return false;

  return true;
}


QByteArray AuthRequest::data(QDataStream *stream) const
{
  if (!json.isEmpty())
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

  if (fields & ExtraInfoField) {
    writer.put<quint8>(OsInfo::type());
    writer.put<quint32>(Ver::current().toUInt());
    writer.put<qint32>(DateTime::tz());
    writer.put(QHostInfo::localHostName());
  }

  return writer.data();
}


void AuthRequest::setStatus(quint8 status)
{
  if (status == Status::Offline)
    status = Status::Online;

  this->status = status;
}