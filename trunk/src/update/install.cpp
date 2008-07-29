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

#include "install.h"


/** [public]
 * 
 */
Install::Install(QObject *parent)
  : QObject(parent)
{
  m_clean = false;
  m_run = false;
  m_appPath = QDir::toNativeSeparators(qApp->applicationDirPath());
  m_s = new QSettings(m_appPath + "/schat.conf", QSettings::IniFormat, this);
  m_ready = m_s->value("Updates/ReadyToInstall", false).toBool();
  m_list = m_s->value("Updates/Files", QStringList()).toStringList();
  if (m_list.isEmpty())
    m_ready = false;
  else
    foreach (QString str, m_list)
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
    m_process.start('"' + m_appPath + "/updates/" + m_queue.dequeue() + '"', QStringList() << "/S" << "-update");
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
  
  if (m_clean) {
    QFile::remove(m_appPath + "/updates/update.xml");
    foreach (QString file, m_list)
      QFile::remove(m_appPath + "/updates/" + file);
  }
  
  if (m_run)
    QProcess::startDetached('"' + m_appPath + "/schat.exe\"");
  
  qApp->quit();
}
