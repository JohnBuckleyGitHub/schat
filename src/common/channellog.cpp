/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include <QCoreApplication>
#include <QDir>
#include <QRegExp>

#include "channellog.h"
#include "text/PlainTextFilter.h"

/*!
 * Конструктор класса ChannelLog.
 */
ChannelLog::ChannelLog(QObject *parent)
  : QObject(parent),
  m_mode(Html),
  m_date(dateStamp())
{
  m_logPath = QCoreApplication::applicationDirPath() + "/log";
}


/*!
 * Конструктор класса ChannelLog.
 */
ChannelLog::ChannelLog(const QString &logPath, QObject *parent)
  : QObject(parent),
  m_mode(Html),
  m_date(dateStamp()),
  m_logPath(logPath)
{
}


QString ChannelLog::htmlFilter(const QString &html, int left, bool strict)
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

  if (PlainTextFilter::filter(out).isEmpty())
    return QString();

  return out;
}


/*!
 * \author © 2002-2008 by the Kopete developers <kopete-devel@kde.org>
 */
static QString makeRegExp(const char *pattern)
{
  const QString urlChar = QLatin1String("\\+\\-\\w\\./\\\\#@&;:=\\?~%_,\\!\\$\\*\\(\\)");
  const QString boundaryStart = QString("(^|[^%1])(").arg(urlChar);
  const QString boundaryEnd = QString(")([^%1]|$)").arg(urlChar);

  return boundaryStart + QLatin1String(pattern) + boundaryEnd;
}


/*!
 * Преобразование простых ссылок в html ссылки.
 *
 * \author © 2002-2008 by the Kopete developers <kopete-devel@kde.org>
 */
QString ChannelLog::parseLinks(const QString &message, bool plain)
{
  if (!plain)
  {
    // < in HTML *always* means start-of-tag
    QStringList entries = message.split(QChar('<'), QString::KeepEmptyParts);

    QStringList::Iterator it = entries.begin();

    // first one is different: it doesn't start with an HTML tag.
    if (it != entries.end()) {
      *it = parseLinks( *it, true);
      ++it;
    }

    for ( ; it != entries.end(); ++it ) {
      QString curr = *it;
      // > in HTML means start-of-tag if and only if it's the first one after a <
      int tagclose = curr.indexOf(QChar('>'));
      // no >: the HTML is broken, but we can cope
      if (tagclose == -1)
        continue;
      QString tag = curr.left(tagclose + 1);
      QString body = curr.mid(tagclose + 1);
      *it = tag + parseLinks(body, true);
    }
    return entries.join(QLatin1String("<"));
  }

  QString result = message;

  // common subpatterns - may not contain matching parens!
  const QString name = QLatin1String("[\\w\\+\\-=_\\.]+");
  const QString userAndPassword = QString("(?:%1(?::%1)?\\@)").arg(name);
  const QString urlChar = QLatin1String("\\+\\-\\w\\./\\\\#@&;:=\\?~%_,\\!\\$\\*\\(\\)");
  const QString urlSection = QString("[%1]+").arg(urlChar);
  const QString domain = QLatin1String("[\\-\\w_]+(?:\\.[\\-\\w_]+)+");

  //Replace http/https/ftp links:
  // Replace (stuff)://[user:password@](linkstuff) with a link
  result.replace(
    QRegExp(makeRegExp("\\w+://%1?\\w%2").arg(userAndPassword, urlSection)),
    QLatin1String("\\1<a href=\"\\2\" title=\"\\2\">\\2</a>\\3"));

  // Replace www.X.Y(linkstuff) with a http: link
  result.replace(
    QRegExp(makeRegExp("%1?www\\.%2%3").arg(userAndPassword, domain, urlSection)),
    QLatin1String("\\1<a href=\"http://\\2\" title=\"http://\\2\">\\2</a>\\3"));

  // Replace \\server\sharename with a file: link
  result.replace(
    QRegExp(makeRegExp("\\\\\\\\%1").arg(urlSection)),
    QLatin1String("\\1<a href=\"file:\\2\" title=\"\\2\">\\2</a>\\3"));

  //Replace Email Links
  // Replace user@domain with a mailto: link
  result.replace(
    QRegExp(makeRegExp("%1@%2").arg(name, domain)),
    QLatin1String("\\1<a href=\"mailto:\\2\" title=\"mailto:\\2\">\\2</a>\\3"));

  //Workaround for Bug 85061: Highlighted URLs adds a ' ' after the URL itself
  // the trailing  &nbsp; is included in the url.
  result.replace(QRegExp(QLatin1String("(<a href=\"[^\"]+)(&nbsp;)(\")")), QLatin1String("\\1\\3"));

  return result;
}


/*!
 * Запись строки в журнал.
 *
 * \param text Строка.
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

    if (m_file.isOpen()) {
      if (m_mode == Html) {
        QString line = "<div><small class='ts'>" + dateTimeStamp() + "</small> " + text + "</div>";
        m_stream << line << endl;
      }
      else
        m_stream << dateTimeStamp() << ' ' << text << endl;
    }
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

  QDir dir(m_logPath + "/" + date);
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());

  bool bom = false;
  QString fileName = m_logPath + "/" + date + '/' + m_channel;
  if (m_mode == Html)
    fileName += ".html";
  else
    fileName += ".log";

  m_file.setFileName(fileName);
  if (!m_file.exists())
    bom = true;

  if (m_file.open(QIODevice::Append)) {
    m_stream.setDevice(&m_file);
    m_stream.setGenerateByteOrderMark(bom);
    m_stream.setCodec("UTF-8");
  }
  else
    return false;

  if (m_mode == Html && bom) {
    m_stream << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << endl;
    m_stream << "<html xmlns=\"http://www.w3.org/1999/xhtml\">" << endl;
    m_stream << "<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />" << endl;
    m_stream << "<style type=\"text/css\">" << endl;
    m_stream << " .sender { color:#185074; }" << endl;
    m_stream << " .me { color:#cd00cd; }" << endl;
    m_stream << " a { text-decoration:none; }" << endl;
    m_stream << " a:link, a:visited { color:#815d53; }" << endl;
    m_stream << " a:active { color:#858e93; }" << endl;
    m_stream << " a span[style] { text-decoration:none !important; color:#815d53 !important; }" << endl;
    m_stream << " .sb { color:#002b3c; background-color:#f5f8ff; border:#dce2e6 solid 1px; margin:1px 8px; padding:1px 4px; }" << endl;
    m_stream << " .oldClientProtocol, .oldServerProtocol, .badNickName, .accessDenied, .disconnect { color:#da251d; }" << endl;
    m_stream << " .ready { color:#6bb521; }" << endl;
    m_stream << " .ts, .preSb, .newUser, .newUser a, .userLeft, .userLeft a, .away, .away a, .newLink, .linkLeave { color:#8797a3; }" << endl;
    m_stream << " .newUser a, .userLeft a { font-weight:bold; }" << endl;
    m_stream << " .info, .changedNick, .changedNick a { color:#5096cf; }" << endl;
    m_stream << " .changedNick a { font-weight:bold; }" << endl;
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
