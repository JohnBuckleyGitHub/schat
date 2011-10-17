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
#include "text/HtmlFilter.h"
#include "text/TokenFilter.h"
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
  if (!data.id.isEmpty())
    m_id = SimpleID::encode(data.id);

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


QString AbstractMessage::quote(const QString &text)
{
  int len = text.length(), c;
  QString res;
  res.reserve(len + 128);

  for (int f = 0; f < len; f++) {
    QChar ch(text[f]);
    ushort uc = ch.unicode();
    if (uc < 32) {
      // control char
      switch (uc) {
        case '\b': res += "\\b"; break;
        case '\f': res += "\\f"; break;
        case '\n': res += "\\n"; break;
        case '\r': res += "\\r"; break;
        case '\t': res += "\\t"; break;
        default:
          res += "\\u";
          for (c = 4; c > 0; c--) {
            ushort n = (uc>>12)&0x0f;
            n += '0'+(n>9?7:0);
            res += (uchar)n;
          }
          break;
      }
    } else {
      // normal char
      switch (uc) {
        case '"': res += "\\\""; break;
        case '\\': res += "\\\\"; break;
        default: res += ch; break;
      }
    }
  }

  return res;
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
  if (text.isEmpty())
    return;

  QStringList filters = TokenFilter::defaults(m_type);
  if (filters.isEmpty()) {
    m_text = text;
    return;
  }

  HtmlFilter filter(parseOptions);
  QList<HtmlToken> tokens = filter.tokenize(text);

  for (int i = 0; i < filters.size(); ++i) {
    TokenFilter::filter(filters.at(i), tokens);
  }

  m_text = HtmlFilter::build(tokens);
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
QString AbstractMessage::appendMessage(QString &html, const QString &func) const
{
  return func + "(\"" + quote(html) + "\");";
}


QString AbstractMessage::button(const QVariant &data) const
{
  if (data.type() != QVariant::Map)
    return QString();

  QVariantMap map = data.toMap();
  if (map.isEmpty())
    return QString();

  if (!map.contains("template") || map.value("template").toString().isEmpty())
    map["template"] = "button";

  QString t = tpl(map.value("template").toString());
  if (t.isEmpty())
    return QString();

  replace(t, map);
  return t;
}


/*!
 * Получения списка переменных вида %var% из строки \p text.
 *
 * \return Список переменных без начальных и конечных символов %.
 */
QStringList AbstractMessage::vars(const QString &text) const
{
  QStringList out;
  QString var;
  int start = 0;
  int end = 0;

  forever {
    start = text.indexOf('%', end);
    if (start == -1)
      break;

    ++start;
    end = text.indexOf('%', start);
    if (end == -1)
      break;

    ++end;

    var = text.mid(start, end - start - 1);
    if (!out.contains(var))
      out.append(var);
  }

  return out;
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
  t.replace(QLatin1String("%user-id%"), SimpleID::encode(m_senderId));
  t.replace(QLatin1String("%user-url%"), UserUtils::toUrl(user, QLatin1String("insert")).toString());
  t.replace(QLatin1String("%user-nick%"), Qt::escape(user->nick()));

  html.replace(QLatin1String("%nick%"), t);
}


void AbstractMessage::replace(QString &text, const QVariantMap map) const
{
  QStringList vars = this->vars(text);
  if (vars.isEmpty())
    return;

  for (int i = 0; i < vars.size(); ++i) {
    text.replace('%' + vars.at(i) + '%', map.value(vars.at(i)).toString());
  }
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
  t.replace(QLatin1String("%message%"), m_text);

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
