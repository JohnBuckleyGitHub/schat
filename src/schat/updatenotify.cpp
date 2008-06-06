/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtCore>

#include "updatenotify.h"


/** [public]
 * 
 */
UpdateNotify::UpdateNotify(QObject *parent)
  : QObject(parent)
{
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
  qDebug() << "void UpdateNotify::execute()";
  m_process.start(m_appPath + "/schat-update.exe -get");
  
}


/** [private slots]
 * 
 */
void UpdateNotify::error(QProcess::ProcessError error)
{
  qDebug() << "UpdateNotify::error()" << error;
  emit done(-1);
  
}


/** [private slots]
 * 
 */
void UpdateNotify::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
  qDebug() << "UpdateNotify::finished()" << exitCode << exitStatus;
  emit done(exitCode);  
}
