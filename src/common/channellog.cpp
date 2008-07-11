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

#include <QtGui>

#include "channellog.h"


/** [public]
 * 
 */
ChannelLog::ChannelLog(QObject *parent)
  : QObject(parent)
{
  m_mode = Html;
  m_date = dateStamp();
  m_appPath = qApp->applicationDirPath();
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
        m_stream << dateTimeStamp() << ' ' << text << endl;
  }
}


/** [public]
 * 
 */
void ChannelLog::setChannel(const QString &channel)
{
  if (m_channel != channel) {
    if (m_file.isOpen())
      m_file.close();
    m_channel = channel;
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
    m_stream << "  .me { color:#f3f; }" << endl;
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
