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
  m_s = new QSettings(qApp->applicationDirPath() + "/schat.conf", QSettings::IniFormat, this);
  m_ready = m_s->value("Updates/ReadyToInstall", false).toBool();
  QStringList list = m_s->value("Updates/Files", QStringList()).toStringList();
  if (list.isEmpty())
    m_ready = false;
  else
    foreach (QString str, list)
      m_queue.enqueue(str);
}


/** [public slots]
 * 
 */
void Install::execute()
{
  qDebug() << "Install::execute()" << m_ready;
  
  if (!m_ready)
    fail();
  
}


/** [private slots]
 * 
 */
void Install::fail()
{
  qDebug() << "Install::fail()";
  
  m_s->setValue("Updates/ReadyToInstall", false);
}


/** [private slots]
 * 
 */
void Install::finished(int exitCode, QProcess::ExitStatus exitStatus)
{

}
