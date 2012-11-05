/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#ifndef CHANNELLOG_H_
#define CHANNELLOG_H_

#include <QFile>
#include <QObject>
#include <QTextStream>
#include <QDateTime>

/*!
 * \brief Универсальный класс для записи в лог каналов.
 *
 * Класс используется в клиенте и сервере и обеспечивает создание отдельный директорий в виде даты для логов.
 * Поддерживаются простой текстовый формат логов таки html-формат.
 * Из имён каналов удаляются символы недопустимые для имён файлов.
 */
class ChannelLog : public QObject {
  Q_OBJECT

public:
  enum Mode {
    Html,
    Plain
  };

  ChannelLog(QObject *parent = 0);
  ChannelLog(const QString &logPath, QObject *parent = 0);
  inline QString channel()                       { return m_channel; }
  inline void setMode(Mode mode)                 { m_mode = mode; }
  static QString htmlFilter(const QString &html, int left = 7000, bool strict = true);
  static QString parseLinks(const QString &message, bool plain = false);
  void msg(const QString &text);
  void setChannel(const QString &channel);

private:
  bool openFile();
  inline static QString dateStamp()              { return QDate::currentDate().toString("yyyy_MM_dd"); }
  inline static QString dateTimeStamp()          { return QDateTime::currentDateTime().toString("(dd.MM.yyyy hh:mm:ss)"); }
  void closeFile();

  Mode m_mode;
  QFile m_file;
  QString m_channel;
  QString m_date;
  QString m_logPath;
  QTextStream m_stream;
};

#endif /*CHANNELLOG_H_*/
