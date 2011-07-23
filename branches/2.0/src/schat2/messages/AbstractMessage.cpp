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

#include <QFile>

#include "messages/AbstractMessage.h"

QHash<QString, QString> AbstractMessage::m_templates;

AbstractMessage::AbstractMessage(int type, const QString &text, const QByteArray &destId, int parseOptions)
  : m_direction(UnknownDirection)
  , m_parseOptions(parseOptions)
  , m_priority(NormalPriority)
  , m_destId(destId)
  , m_timestamp(0)
  , m_type(type)
{
  setText(text, parseOptions);
}


/*!
 * Получение текста шаблона, текст кэшируется в статическом члене \p m_templates.
 *
 * \param fileName Имя файла шаблона без пути и расширения.
 */
QString AbstractMessage::tpl(const QString &fileName)
{
  if (m_templates.contains(fileName))
    return m_templates.value(fileName);

  QFile file(":/html/templates/" + fileName + ".html");
  if (file.open(QIODevice::ReadOnly)) {
    QString tpl = file.readAll();
    m_templates.insert(fileName, tpl);
    return tpl;
  }
  else
    return tpl("generic");
}


/*!
 * JavaScript код.
 */
QString AbstractMessage::js() const
{
  QString html = tpl("generic");

  html.replace("%extra%", "generic");

  replaceTimeStamp(html);
  replaceText(html);

  return appendMessage(html);
}


void AbstractMessage::setText(const QString &text, int parseOptions)
{
  Q_UNUSED(parseOptions);
  m_text = text;
}


QDateTime AbstractMessage::dateTime() const
{
  if (m_timestamp == 0)
    return QDateTime::currentDateTime();

  #if QT_VERSION >= 0x040700
  return QDateTime::fromMSecsSinceEpoch(m_timestamp);
  #else
  return QDateTime::fromTime_t(m_timestamp / 1000);
  #endif
}


/*!
 * Добавление сообщения в чат.
 */
QString AbstractMessage::appendMessage(QString &html) const
{
  html.replace("\"","\\\"");
  html.replace("\n","\\n");
  return "appendMessage(\"" + html + "\");";
}


/*!
 * Установка текста.
 */
void AbstractMessage::replaceText(QString &html) const
{
  QString msg = m_text;
  html.replace("%message%", msg.replace("\\", "\\\\").remove('\r').replace("%", "&#37;"));
}


/*!
 * Установка времени в HTML шаблоне.
 */
void AbstractMessage::replaceTimeStamp(QString &html) const
{
  QDateTime dt = dateTime();

  html.replace("%time%", dt.toString("hh:mm"));
  html.replace("%seconds%", dt.toString(":ss"));
}
