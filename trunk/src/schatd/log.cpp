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

#include "log.h"


/** [public]
 * 
 */
Log::Log(QObject *parent)
  : QObject(parent)
{
  m_appPath = qApp->applicationDirPath();
}


/** [public]
 * 
 */
bool Log::init()
{
  QDir dir(m_appPath + "/log");
  if (!dir.exists())
    dir.mkdir(m_appPath + "/log");
  
  bool bom = false;
  
  m_file.setFileName(m_appPath + "/log/schatd.log");
  if (!m_file.exists())
    bom = true;
  
  if (!m_file.open(QIODevice::Append))
    return false;
  else {
    m_stream.setDevice(&m_file);
    m_stream.setGenerateByteOrderMark(bom);
    m_stream.setCodec("UTF-8");
  }   
  
  return true;
}


/** [public]
 * 
 */
void Log::append(const QString &text)
{
  m_stream << QDateTime(QDateTime::currentDateTime()).toString("(yyyy.MM.dd hh:mm:ss) ") << text << endl;
}
