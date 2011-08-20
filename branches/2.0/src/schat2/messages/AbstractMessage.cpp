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
#include <QTextDocument>

#include "ChatCore.h"
#include "client/SimpleClient.h"
#include "messages/AbstractMessage.h"
#include "net/packets/message.h"
#include "ui/UserUtils.h"

QHash<QString, QString> AbstractMessage::m_templates;

AbstractMessage::AbstractMessage(const QString &type, const MessageData &data, int parseOptions)
  : m_direction(UnknownDirection)
  , m_parseOptions(parseOptions)
  , m_priority(NormalPriority)
  , m_destId(data.destId())
  , m_senderId(data.senderId)
  , m_timestamp(data.timestamp)
  , m_bodyTpl(QLatin1String("body"))
  , m_template(QLatin1String("generic"))
  , m_timeTpl(QLatin1String("time"))
  , m_type(type)
{
  if (data.name)
    m_id = m_senderId.toHex() + QLatin1String("-") + QString::number(data.name);

  setText(data.text, parseOptions);
}


AbstractMessage::AbstractMessage(const QString &type, const QString &text, const QByteArray &destId, int parseOptions)
  : m_direction(UnknownDirection)
  , m_parseOptions(parseOptions)
  , m_priority(NormalPriority)
  , m_destId(destId)
  , m_timestamp(0)
  , m_bodyTpl(QLatin1String("body"))
  , m_template(QLatin1String("generic"))
  , m_timeTpl(QLatin1String("time"))
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

  QFile file(QLatin1String(":/html/templates/") + fileName + QLatin1String(".html"));
  if (file.open(QIODevice::ReadOnly)) {
    QString tpl = file.readAll();
    m_templates.insert(fileName, tpl);
    return tpl;
  }
  else
    return tpl(QLatin1String("generic"));
}


/*!
 * JavaScript код.
 *
 * \param add true если нужно добавить код вставки в станицу.
 */
QString AbstractMessage::js(bool add) const
{
  QString html = tpl(m_template);
  type(html);
  id(html);
  extra(html);
  time(html);
  nick(html);
  text(html);

  if (add)
    return appendMessage(html);
  else
    return html;
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
  html.replace(QLatin1String("\""), QLatin1String("\\\""));
  html.replace(QLatin1String("\n"), QLatin1String("\\n"));
  return QLatin1String("appendMessage(\"") + html + QLatin1String("\");");
}


void AbstractMessage::extra(QString &html) const
{
  html.replace(QLatin1String("%extra%"), m_extra);
}


void AbstractMessage::id(QString &html) const
{
  if (m_id.isEmpty())
    html.remove(QLatin1String("id=\"%message-id%\""));
  else
    html.replace(QLatin1String("%message-id%"), m_id);
}


void AbstractMessage::nick(QString &html) const
{
  if (!html.contains(QLatin1String("%nick%")))
    return;

  ClientUser user = UserUtils::user(m_senderId);
  if (!user) {
    html.remove(QLatin1String("%nick%"));
    return;
  }

  QString t = tpl(QLatin1String("nick"));
  t.replace(QLatin1String("%user-id%"), SimpleID::toBase64(m_senderId));
  t.replace(QLatin1String("%user-url%"), UserUtils::toUrl(user, QLatin1String("insert")).toString());
  t.replace(QLatin1String("%user-nick%"), Qt::escape(user->nick()));

  html.replace(QLatin1String("%nick%"), t);
}


/*!
 * Установка тела сообщения.
 */
void AbstractMessage::text(QString &html) const
{
  if (!html.contains(QLatin1String("%body%")))
    return;

  if (m_text.isEmpty()) {
    html.remove(QLatin1String("%body%"));
    return;
  }

  QString t = tpl(m_bodyTpl);

  QString text = m_text;
  t.replace(QLatin1String("%message%"), text.replace("\\", "\\\\").remove('\r').replace("%", "&#37;"));

  html.replace(QLatin1String("%body%"), t);
}


/*!
 * Установка времени сообщения.
 */
void AbstractMessage::time(QString &html) const
{
  if (!html.contains(QLatin1String("%time%")))
    return;

  QString t = tpl(m_timeTpl);
  QDateTime dt = dateTime();
  if (t.contains(QLatin1String("%date%")))
    t.replace(QLatin1String("%date%"), dt.toString(QLatin1String("dd:MM:yyyy")));

  t.replace(QLatin1String("%time%"), dt.toString(QLatin1String("hh:mm")));
  t.replace(QLatin1String("%seconds%"), dt.toString(QLatin1String(":ss")));

  html.replace(QLatin1String("%time%"), t);
}


void AbstractMessage::type(QString &html) const
{
  html.replace(QLatin1String("%message-type%"), m_type);
}
