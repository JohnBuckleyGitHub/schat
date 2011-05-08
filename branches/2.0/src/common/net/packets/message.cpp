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


void MessageData::autoSetOptions()
{
  options = NoOptions;

  if (!command.isEmpty())
    options |= ControlOption;

  if (name > 0)
    options |= NameOption;

  if (!text.isEmpty())
    options |= TextOption;
}


MessageWriter::MessageWriter(QDataStream *stream, const MessageData &data)
  : PacketWriter(stream, Protocol::MessagePacket, data.senderId, data.destId)
{
  put<quint8>(data.options);

  if (data.options & MessageData::NameOption)
    put<quint64>(data.name);

  if (data.options & MessageData::ControlOption)
    put(data.command);

  if (data.options & MessageData::TextOption)
    put(data.text);
}


MessageReader::MessageReader(PacketReader *reader)
{
  data.senderId = reader->sender();
  data.destId = reader->dest();
  data.options = reader->get<quint8>();

  if (data.options & MessageData::NameOption)
    data.name = reader->get<quint64>();

  if (data.options & MessageData::ControlOption)
    data.command = reader->text();

  if (data.options & MessageData::TextOption)
    data.text = reader->text();
}


QString MessageUtils::toPlainText(const QString &text)
{
  QString out = text;
  out.replace(QLatin1String("<br />"), QLatin1String("\n"), Qt::CaseInsensitive);
  out.remove(QLatin1String("</span>"), Qt::CaseInsensitive);
  out.remove(QRegExp(QLatin1String("<[^>]*>")));

  out.replace(QLatin1String("&gt;"),   QLatin1String(">"));
  out.replace(QLatin1String("&lt;"),   QLatin1String("<"));
  out.replace(QLatin1String("&quot;"), QLatin1String("\""));
  out.replace(QLatin1String("&nbsp;"), QLatin1String(" "));
  out.replace(QLatin1String("&amp;"),  QLatin1String("&"));
  out.replace(QChar(QChar::Nbsp),      QLatin1String(" "));
  out = out.trimmed();
  return out;
}
