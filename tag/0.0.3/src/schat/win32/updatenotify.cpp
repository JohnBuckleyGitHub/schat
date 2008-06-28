/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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
    m_process.start('"' + m_appPath + "/schat-update.exe\" -get " + m_settings->updateUrl); 
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
