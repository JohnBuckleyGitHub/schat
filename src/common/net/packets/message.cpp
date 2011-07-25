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


QString MessageUtils::htmlFilter(const QString &html, int left, bool strict)
{
  QString out = html;

  /// Очищает html документ от неотображаемой информации.
  QRegExp badStuff(QLatin1String("<![^<>]*>|<head[^<>]*>.*</head[^<>]*>|</?html[^<>]*>|</?body[^<>]*>|</?p[^<>]*>"));
  badStuff.setCaseSensitivity(Qt::CaseInsensitive);
  out.remove(badStuff);
  out = out.trimmed();

  if (out.isEmpty())
    return "";

  out.remove('\r');

  /// Обрезает до максимальной длинны.
  if (left)
    out = out.left(left);

  if (!strict)
    return out;

  /// Удаляет запрещённые теги.
  QList<QLatin1String> otherBadTags;
  otherBadTags << QLatin1String("address")
               << QLatin1String("big")
               << QLatin1String("blockquote")
               << QLatin1String("center")
               << QLatin1String("dd")
               << QLatin1String("div")
               << QLatin1String("dl")
               << QLatin1String("dt")
               << QLatin1String("font")
               << QLatin1String("h1")
               << QLatin1String("h2")
               << QLatin1String("h3")
               << QLatin1String("h4")
               << QLatin1String("h5")
               << QLatin1String("h6")
               << QLatin1String("hr")
               << QLatin1String("kbd")
               << QLatin1String("li")
               << QLatin1String("ol")
               << QLatin1String("qt")
               << QLatin1String("small")
               << QLatin1String("sub")
               << QLatin1String("sup")
               << QLatin1String("table")
               << QLatin1String("tbody")
               << QLatin1String("td")
               << QLatin1String("tfoot")
               << QLatin1String("th")
               << QLatin1String("thead")
               << QLatin1String("tr")
               << QLatin1String("img")
               << QLatin1String("ul");

  foreach (QString tag, otherBadTags) {
    badStuff.setPattern(QString("</?%1[^<>]*>").arg(tag));
    out.remove(badStuff);
  }

  /// Удаляет пустые ссылки.
  badStuff.setPattern(QLatin1String("<a[^<]*>[\\s]*</a>"));
  out.remove(badStuff);

  out.replace("  ", "&nbsp;&nbsp;");

  /// Заменяет перенос строк на соответствующий html код.
  out.replace(QLatin1String("\n"), QLatin1String("<br />"));

  /// Заменяет двойные переносы строк на одинарные.
  while (out.contains(QLatin1String("<br /><br /><br />")))
    out.replace(QLatin1String("<br /><br /><br />"), QLatin1String("<br /><br />"));

  /// Удаляет код переноса строки если тот находится в конце сообщения.
  out.replace(QLatin1String("<br /><br /></span>"), QLatin1String("<br /></span>"));
  out.replace(QLatin1String("<br /></span>"), QLatin1String("</span>"));

  if (out.endsWith(QLatin1String("<br /><br />")))
    out = out.left(out.size() - 12);
  if (out.endsWith(QLatin1String("<br />")))
    out = out.left(out.size() - 6);

  /// Удаляет запрещённые css стили.
  /// \todo Эта функция также удалит заданные селекторы из текста, что не допустимо.
  badStuff.setPattern("\\s?font-size:[^;]*;|\\s?background-color:[^;]*;|\\s?font-family:[^;]*;");
  out.remove(badStuff);

  if (toPlainText(out).isEmpty())
    return "";

  return out;
}


/*!
 * Преобразует HTML текст в простой текст.
 */
QString MessageUtils::toPlainText(const QString &text)
{
  QString out = text;
  out.replace(QLatin1String("<br />"), QLatin1String("\n"), Qt::CaseInsensitive);
  out.remove(QLatin1String("</span>"), Qt::CaseInsensitive);
  out.remove(QRegExp(QLatin1String("<style.*</style>")));
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
