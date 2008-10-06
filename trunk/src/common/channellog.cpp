/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore>

#include "channellog.h"

/*!
 * \class ChannelLog
 * \brief Универсальный класс для записи в лог каналов.
 *
 * Класс используется в клиенте и сервере и обеспечивает создание отдельный директорий в виде даты для логов.
 * Поддерживаются простой текстовый формат логов таки html-формат.
 * Из имён каналов удаляются символы недопустимые для имён файлов.
 */

/*!
 * \brief Конструктор класса ChannelLog.
 */
ChannelLog::ChannelLog(QObject *parent)
  : QObject(parent)
{
  m_mode = Html;
  m_date = dateStamp();
  m_appPath = qApp->applicationDirPath();
}


QString ChannelLog::htmlFilter(const QString &html, int left)
{
  QString out = html;

  /// Очищает html документ от неотображаемой информации.
  QRegExp badStuff(QLatin1String("<![^<>]*>|<head[^<>]*>.*</head[^<>]*>|</?html[^<>]*>|</?body[^<>]*>|</?p[^<>]*>"));
  badStuff.setCaseSensitivity(Qt::CaseInsensitive);
  out.remove(badStuff);
  out = out.trimmed();

  if (out.isEmpty())
    return "";

  /// Обрезает до максимальной длинны.
  out = out.left(left);

  /// Удаляет запрещённые теги.
  badStuff.setCaseSensitivity(Qt::CaseInsensitive);
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

  /// Заменяет перенос строк на соответствующий html код.
  out.replace(QLatin1String("\n"), QLatin1String("<br />"));

  /// Заменяет двойные переносы строк на одинарные.
  while (out.contains(QLatin1String("<br /><br />")))
    out.replace(QLatin1String("<br /><br />"), QLatin1String("<br />"));

  /// Удаляет код переноса строки если тот находится в конце сообщения.
  out.replace(QLatin1String("<br /></span>"), QLatin1String("</span>"));
  if (out.endsWith(QLatin1String("<br />")))
    out.left(out.size() - 6);

  /// Удаляет запрещённые css стили.
  badStuff.setPattern(QLatin1String("font-size:[^;]*;|background-color:[^;]*;|font-family:[^;]*;"));
  out.remove(badStuff);

  if (toPlainText(out).isEmpty())
    return "";

  return out.simplified();
}


QString ChannelLog::toPlainText(const QString &str)
{
  QString out = str;
  out.replace(QLatin1String("<br />"), QLatin1String("\n"), Qt::CaseInsensitive);
  out.remove(QLatin1String("</span>"), Qt::CaseInsensitive);
  out.remove(QRegExp(QLatin1String("</?span[^<>]*>|</?a[^<>]*>")));

  out.replace(QLatin1String("&gt;"),   QLatin1String(">"));
  out.replace(QLatin1String("&lt;"),   QLatin1String("<"));
  out.replace(QLatin1String("&quot;"), QLatin1String("\""));
  out.replace(QLatin1String("&nbsp;"), QLatin1String(" "));
  out.replace(QLatin1String("&amp;"),  QLatin1String("&"));
  out = out.trimmed();
  return out;
}


void ChannelLog::msg(const QString &text)
{
  if (!m_channel.isEmpty()) {
    if (!m_file.isOpen())
      openFile();

    if (m_date != dateStamp()) {
      closeFile();
      openFile();
    }

    if (m_file.isOpen())
      if (m_mode == Html) {
        QString line = "<div><small class='gr'>" + dateTimeStamp() + "</small> " + text + "</div>";
        m_stream << line << endl;
      }
      else
        m_stream << dateTimeStamp() << ' ' << toPlainText(text) << endl;
  }
}


void ChannelLog::setChannel(const QString &channel)
{
  QString _channel = channel;
  #ifdef Q_OS_WIN
    _channel.replace(QRegExp(QLatin1String("COM[1-9]")), QLatin1String("COM_"));
    _channel.replace(QRegExp(QLatin1String("LPT[1-9]")), QLatin1String("LPT_"));
    _channel.replace(QLatin1String("CON"), QLatin1String("CO_"));
    _channel.replace(QLatin1String("NUL"), QLatin1String("NU_"));
    _channel.replace(QLatin1String("AUX"), QLatin1String("AU_"));
    _channel.replace(QLatin1String("PRN"), QLatin1String("PR_"));
  #endif
  _channel.replace(QRegExp(QLatin1String("[?\"/\\\\<>*|:]")), QLatin1String("_"));

  if (m_channel != _channel) {
    if (m_file.isOpen())
      m_file.close();
    m_channel = _channel;
  }
}


bool ChannelLog::openFile()
{
  QString date = dateStamp();

  QDir dir(m_appPath + "/log/" + date);
  if (!dir.exists())
    dir.mkpath(m_appPath + "/log/" + date);

  bool bom = false;
  QString fileName = m_appPath + "/log/" + date + '/' + m_channel;
  if (m_mode == Html)
    fileName += ".html";
  else
    fileName += ".log";

  m_file.setFileName(fileName);
  if (!m_file.exists())
    bom = true;

  if (!m_file.open(QIODevice::Append))
    return false;
  else {
    m_stream.setDevice(&m_file);
    m_stream.setGenerateByteOrderMark(bom);
    m_stream.setCodec("UTF-8");
  }

  if (m_mode == Html && bom) {
    m_stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << endl;
    m_stream << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl;
    m_stream << "<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />" << endl;
    m_stream << "<style type=\"text/css\">" << endl;
    m_stream << " .gr { color:#90a4b3; }" << endl;
    m_stream << " .green { color:#6bb521; }" << endl;
    m_stream << " .err { color:#da251d; }" << endl;
    m_stream << " .info { color:#5096cf; }" << endl;
    m_stream << " .me { color:#cd00cd; }" << endl;
    m_stream << " a { color:#1a4d82; text-decoration:none; }" << endl;
    m_stream << "</style></head><body>" << endl;
  }

  m_date = date;
  return true;
}


/** [private]
 *
 */
void ChannelLog::closeFile()
{
  if (m_file.isOpen()) {
    if (m_mode == Html)
      m_stream << "</body></html>" << endl;
    m_file.close();
  }
}
