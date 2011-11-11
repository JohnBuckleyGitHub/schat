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

#include "net/PacketReader.h"
#include "net/packets/notices.h"
#include "net/packets/message.h"
#include "net/PacketWriter.h"

/*!
 * Базовый конструктор.
 */
Notice::Notice(const QByteArray &sender, const QByteArray &dest, const QString &command, const QVariant &data, quint64 time, const QByteArray &id)
  : m_sender(sender)
  , m_dest(QList<QByteArray>() << dest)
  , m_fields(0)
  , m_version(0)
  , m_status(OK)
  , m_time(time)
  , m_id(id)
  , m_command(command)
  , m_data(data)
{
  m_type = GenericType;

  if (SimpleID::typeOf(m_id) == SimpleID::MessageId)
    m_fields |= IdField;

  if (!data.isNull())
    m_fields |= JSonField;
}


/*!
 * Базовый конструктор.
 */
Notice::Notice(const QByteArray &sender, const QList<QByteArray> &dest, const QString &command, const QVariant &data, quint64 time, const QByteArray &id)
  : m_sender(sender)
  , m_dest(dest)
  , m_fields(0)
  , m_version(0)
  , m_status(OK)
  , m_time(time)
  , m_id(id)
  , m_command(command)
  , m_data(data)
{
  m_type = GenericType;

  if (SimpleID::typeOf(m_id) == SimpleID::MessageId)
    m_fields |= IdField;

  if (!data.isNull())
    m_fields |= JSonField;
}


/*!
 * Конструктор чтения.
 */
Notice::Notice(quint16 type, PacketReader *reader)
  : m_sender(reader->sender())
  , m_dest(reader->destinations())
  , m_type(type)
  , m_fields(0)
{
  m_fields = reader->get<quint8>();
  m_version = reader->get<quint8>();
  m_status = reader->get<quint16>();
  m_time = reader->get<qint64>();

  if (m_fields & IdField)
    m_id = reader->id();

  m_command = reader->text();

  if (m_fields & JSonField)
    m_raw = reader->get<QByteArray>();

  if (m_fields & TextField)
    m_text = reader->text();
}


bool Notice::isValid() const
{
  if (m_command.isEmpty())
    return false;

  if (m_fields & IdField && SimpleID::typeOf(m_id) != SimpleID::MessageId)
    return false;

  if (m_fields & JSonField && (m_data.isNull() && m_raw.isEmpty()))
    return false;

  if (m_fields & TextField && m_text.isEmpty())
    return false;

  return true;
}


/*!
 * Запись пакета.
 */
QByteArray Notice::data(QDataStream *stream, bool echo) const
{
  PacketWriter writer(stream, Protocol::NoticePacket, m_sender, m_dest, echo);
  writer.put(m_type);
  writer.put(m_fields);
  writer.put(m_version);
  writer.put(m_status);
  writer.put(m_time);

  if (m_fields & IdField)
    writer.putId(m_id);

  writer.put(m_command);

  if (m_fields & JSonField)
    writer.put(m_data);

  if (m_fields & TextField)
    writer.put(m_text);

  return writer.data();
}


/*!
 * Получение JSON данных пакета.
 */
QVariant Notice::json() const
{
  if (!m_raw.isEmpty())
    return SimpleJSon::parse(m_raw);

  return m_data;
}


QString Notice::status(int status)
{
  switch (status) {
    case OK:
      return QObject::tr("OK");

    case BadRequest:
      return QObject::tr("Bad Request");

    case Unauthorized:
      return QObject::tr("Unauthorized");

    case Forbidden:
      return QObject::tr("Forbidden");

    case NotFound:
      return QObject::tr("Not Found");

    case UserAlreadyExists:
      return QObject::tr("User Already Exists");

    case UserNotExists:
      return QObject::tr("User Not Exists");

    case NickAlreadyUse:
      return QObject::tr("Nick Already In Use");

    case UserOffline:
      return QObject::tr("User Offline");

    case Conflict:
      return QObject::tr("Conflict");

    case InternalError:
      return QObject::tr("Internal Error");

    case NotImplemented:
      return QObject::tr("Not Implemented");

    case BadGateway:
      return QObject::tr("Bad Gateway");

    case ServiceUnavailable:
      return QObject::tr("Service Unavailable");

    case GatewayTimeout:
      return QObject::tr("Gateway Timeout");

    default:
      return QObject::tr("Unknown");
  }
}


void Notice::setText(const QString &text)
{
  if (text.isEmpty())
    return;

  m_fields |= TextField;
  m_text = text;
}
