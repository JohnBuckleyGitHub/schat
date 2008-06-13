/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#include <QtGui>

#include "install.h"


/** [public]
 * 
 */
Install::Install(QObject *parent)
  : QObject(parent)
{
  m_appPath = QDir::toNativeSeparators(qApp->applicationDirPath());
  m_s = new QSettings(m_appPath + "/schat.conf", QSettings::IniFormat, this);
  m_ready = m_s->value("Updates/ReadyToInstall", false).toBool();
  QStringList list = m_s->value("Updates/Files", QStringList()).toStringList();
  if (list.isEmpty())
    m_ready = false;
  else
    foreach (QString str, list)
      m_queue.enqueue(str);
  
  connect(&m_process, SIGNAL(error(QProcess::ProcessError)), SLOT(fail()));
  connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(finished(int, QProcess::ExitStatus)));
}


/** [public slots]
 * 
 */
void Install::execute()
{
  if (!m_ready) {
    fail();
    return;
  }
  
  if (!m_queue.isEmpty())
    m_process.start('"' + m_appPath + "/updates/" + m_queue.dequeue() + '"', QStringList() << "/S" << ("/D=" + m_appPath));
}


/** [private slots]
 * 
 */
void Install::fail()
{
  m_s->setValue("Updates/ReadyToInstall", false);
  qApp->exit(1);
}


/** [private slots]
 * 
 */
void Install::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
  if (exitStatus == QProcess::CrashExit || exitCode != 0) {
    fail();
    return;
  }
  
  if (m_queue.isEmpty())
    done();
  else
    execute();
}


/** [private]
 * 
 */
void Install::done()
{
  m_s->setValue("Updates/ReadyToInstall", false);
  qApp->quit();
}
