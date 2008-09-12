/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.com)
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
  QRegExp badStuff("<![^<>]*>|<head[^<>]*>.*</head[^<>]*>|</?html[^<>]*>|</?body[^<>]*>|</?p[^<>]*>");
  badStuff.setCaseSensitivity(Qt::CaseInsensitive);
  out.remove(badStuff);
  out = out.trimmed();

  if (out.isEmpty())
    return "";

  /// Обрезает до максимальной длинны.
  out = out.left(left);

  /// Удаляет запрещённые теги.
  badStuff.setCaseSensitivity(Qt::CaseInsensitive);
  QStringList otherBadTags;
  otherBadTags << "address"
               << "big"
               << "blockquote"
               << "center"
               << "dd"
               << "div"
               << "dl"
               << "dt"
               << "font"
               << "h1"
               << "h2"
               << "h3"
               << "h4"
               << "h5"
               << "h6"
               << "hr"
               << "kbd"
               << "li"
               << "ol"
               << "qt"
               << "small"
               << "sub"
               << "sup"
               << "table"
               << "tbody"
               << "td"
               << "tfoot"
               << "th"
               << "thead"
               << "tr"
               << "img"
               << "ul";

  foreach (QString tag, otherBadTags) {
    badStuff.setPattern(QString("</?%1[^<>]*>").arg(tag));
    out.remove(badStuff);
  }

  /// Удаляет пустые ссылки.
  badStuff.setPattern("<a[^<]*>[\\s]*</a>");
  out.remove(badStuff);

  /// Заменяет перенос строк на соответствующий html код.
  out.replace(QChar('\n'), "<br />");

  /// Заменяет двойные переносы строк на одинарные.
  while (out.contains("<br /><br />"))
    out.replace("<br /><br />", "<br />");

  /// Удаляет код переноса строки если тот находится в конце сообщения.
  out.replace("<br /></span>", "</span>");
  if (out.endsWith("<br />"))
    out.left(out.size() - 6);

  /// Удаляет запрещённые css стили.
  badStuff.setPattern("font-size:[^;]*;|background-color:[^;]*;|font-family:[^;]*;");
  out.remove(badStuff);

  if (toPlainText(out).isEmpty())
    return "";

  return out.simplified();
}


QString ChannelLog::toPlainText(const QString &str)
{
  QString out = str;
  out.replace("<br />", QChar('\n'), Qt::CaseInsensitive);
  out.remove("</span>", Qt::CaseInsensitive);
  out.remove(QRegExp("</?span[^<>]*>|</?a[^<>]*>"));

  out.replace("&gt;", ">");
  out.replace("&lt;", "<");
  out.replace("&quot;", "\"");
  out.replace("&nbsp;", " ");
  out.replace("&amp;", "&");
  out = out.trimmed();
  return out;
}


/** [public]
 * 
 */
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


/** [public]
 * 
 */
void ChannelLog::setChannel(const QString &channel)
{
  QString _channel = channel;
  #ifdef Q_OS_WIN
  _channel.replace(QRegExp("COM[1-9]"), "COM_");
  _channel.replace(QRegExp("LPT[1-9]"), "LPT_");
  _channel.replace("CON", "CO_");
  _channel.replace("NUL", "NU_");
  _channel.replace("AUX", "AU_");
  _channel.replace("PRN", "PR_");
  #endif
  _channel.replace(QRegExp("[?\"/\\\\<>*|:]"), "_");
  
  if (m_channel != _channel) {
    if (m_file.isOpen())
      m_file.close();
    m_channel = _channel;
  }  
}


/** [private]
 * 
 */
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
    m_stream << "  .gr { color:#90a4b3; }" << endl;
    m_stream << "  .green { color:#6bb521; }" << endl;
    m_stream << "  .err { color:#da251d; }" << endl;
    m_stream << "  .info { color:#5096cf; }" << endl;
    m_stream << "  .me { color:#cd00cd; }" << endl;
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
