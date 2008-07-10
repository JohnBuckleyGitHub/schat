/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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

#ifndef CHANNELLOG_H_
#define CHANNELLOG_H_

#include <QFile>
#include <QObject>
#include <QTextStream>
#include <QDateTime>

class ChannelLog : public QObject {
  Q_OBJECT
  
public:
  enum Mode {
    Html,
    Plain
  };
  
  ChannelLog(QObject *parent = 0);
  inline QString channel()                       { return m_channel; }
  inline void setChannel(const QString &channel) { m_channel = channel; }
  inline void setMode(Mode mode)                 { m_mode = mode; }
  void msg(const QString &text);
  
private:
  bool openFile();
  inline static QString dateStamp()              { return QDate::currentDate().toString("yyyy_MM_dd"); }
  inline static QString dateTimeStamp()          { return QDateTime::currentDateTime().toString("(dd.MM.yyyy hh:mm:ss)"); }
  void closeFile();
  
  Mode m_mode;
  QFile m_file;
  QString m_appPath;
  QString m_channel;
  QString m_date;
  QTextStream m_stream;
};

#endif /*CHANNELLOG_H_*/
