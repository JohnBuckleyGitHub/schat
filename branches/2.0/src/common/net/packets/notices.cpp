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

MessageNotice::MessageNotice(quint16 type, PacketReader *reader)
  : AbstractNotice(type, reader->sender(), reader->destinations())
  , m_valid(false)
  , m_fields(0)
  , m_status(0)
  , m_error(0)
{
  m_fields = reader->get<quint8>();
  m_status = reader->get<quint8>();
  m_error = reader->get<quint8>();
  m_id = reader->id();

  if (SimpleID::typeOf(m_id) != SimpleID::MessageId)
    return;

  m_valid = true;
}


MessageNotice::MessageNotice(quint8 status, MessageData *data, quint8 error)
  : AbstractNotice(MessageNoticeType, data->senderId, data->dest)
  , m_valid(false)
  , m_fields(0)
  , m_status(status)
  , m_error(error)
  , m_id(data->id)
{
  if (SimpleID::typeOf(m_id) != SimpleID::MessageId)
    return;

  m_valid = true;
}


MessageNotice::MessageNotice(quint8 status, const QByteArray &sender, const QByteArray &dest, const QByteArray &id, quint8 error)
  : AbstractNotice(MessageNoticeType, sender, dest)
  , m_valid(false)
  , m_fields(0)
  , m_status(status)
  , m_error(error)
  , m_id(id)
{
  if (SimpleID::typeOf(id) != SimpleID::MessageId)
    return;

  m_valid = true;
}


QByteArray MessageNotice::data(QDataStream *stream) const
{
  PacketWriter writer(stream, Protocol::NoticePacket, m_sender, m_dest);
  writer.put(m_type);
  writer.put(m_fields);
  writer.put(m_status);
  writer.put(m_error);
  writer.putId(m_id);

  return writer.data();
}


NoticeData::NoticeData(quint16 type, const QByteArray &senderId, const QByteArray &destId, const QString &text)
  : destId(destId)
  , senderId(senderId)
  , timestamp(0)
  , type(type)
  , messageName(0)
  , param1(0)
  , param2(0)
  , text(text)
{
}

NoticeData::NoticeData(const QByteArray &senderId, const QByteArray &destId, quint16 type, quint64 messageName, quint8 param1)
  : destId(destId)
  , senderId(senderId)
  , timestamp(0)
  , type(type)
  , messageName(messageName)
  , param1(param1)
  , param2(0)
{
}

NoticeWriter::NoticeWriter(QDataStream *stream, const NoticeData &data)
  : PacketWriter(stream, Protocol::NoticePacket, data.senderId, data.destId)
{
  put(data.type);
  put(data.param1);
  put(data.param2);

  if (data.type == NoticeData::MessageDelivered || data.type == NoticeData::MessageRejected)
    put(data.messageName);

  put(data.text);
}


NoticeReader::NoticeReader(PacketReader *reader)
{
  data.senderId = reader->sender();
  data.destId = reader->dest();
  data.type = reader->get<quint16>();
  data.param1 = reader->get<quint8>();
  data.param2 = reader->get<quint8>();

  if (data.type == NoticeData::MessageDelivered || data.type == NoticeData::MessageRejected)
    data.messageName = reader->get<quint64>();

  data.text = reader->text();
}
