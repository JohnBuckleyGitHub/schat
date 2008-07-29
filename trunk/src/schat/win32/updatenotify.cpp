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

#include "settings.h"
#include "updatenotify.h"


/** [public]
 * 
 */
UpdateNotify::UpdateNotify(Settings *settings, QObject *parent)
  : QObject(parent)
{
  m_settings = settings;
  m_appPath = qApp->applicationDirPath();
  m_process.setWorkingDirectory(m_appPath);
  
  connect(&m_process, SIGNAL(error(QProcess::ProcessError)), SLOT(error(QProcess::ProcessError)));
  connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(finished(int, QProcess::ExitStatus)));
}


/** [public slots]
 * 
 */
void UpdateNotify::execute()
{
  if (m_process.state() == QProcess::NotRunning)
    m_process.start('"' + m_appPath + "/update.exe\" -get " + m_settings->updateUrl); 
}


/** [private slots]
 * 
 */
void UpdateNotify::error(QProcess::ProcessError /*error*/)
{
  emit done(-1);  
}


/** [private slots]
 * 
 */
void UpdateNotify::finished(int exitCode, QProcess::ExitStatus /*exitStatus*/)
{
  emit done(exitCode);  
}
