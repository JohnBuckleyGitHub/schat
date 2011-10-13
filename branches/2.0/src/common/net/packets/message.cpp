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
#include "net/packets/message.h"
#include "net/PacketWriter.h"
#include "net/Protocol.h"

MessageWriter::MessageWriter(QDataStream *stream, const MessageData &data, bool echo)
  : PacketWriter(stream, Protocol::MessagePacket, data.senderId, data.dest, echo)
{
  put<quint8>(data.options);

  if (data.options & MessageData::TimeOption)
    put<quint64>(data.timestamp);

  if (data.options & MessageData::ExtraFlagsOption)
    put<quint16>(data.flags);

  if (data.options & MessageData::IdOption)
    putId(data.id);

  if (data.options & MessageData::ControlOption)
    put(data.command);

  if (data.options & MessageData::TextOption)
    put(data.text);

  if (data.options & MessageData::JSonOption)
    put(data.json);
}


MessageReader::MessageReader(PacketReader *reader)
{
  data.senderId = reader->sender();
  data.dest = reader->destinations();
  data.options = reader->get<quint8>();

  if (data.options & MessageData::TimeOption)
    data.timestamp = reader->get<quint64>();

  if (data.options & MessageData::ExtraFlagsOption)
    data.flags = reader->get<quint16>();

  if (data.options & MessageData::IdOption)
    data.id = reader->id();

  if (data.options & MessageData::ControlOption)
    data.command = reader->text();

  if (data.options & MessageData::TextOption)
    data.text = reader->text();

  if (data.options & MessageData::JSonOption)
    data.json = reader->json();
}


bool MessageUtils::remove(const QString &cmd, QString &msg)
{
  QString c = cmd;
  int index = msg.indexOf(c, 0, Qt::CaseInsensitive);
  if (index == -1 && c.endsWith(' ')) {
    c = c.left(c.size() - 1);
    index = msg.indexOf(c, 0, Qt::CaseInsensitive);
  }

  if (index == -1)
    return false;

  msg.remove(index, c.size());
  return true;
}
