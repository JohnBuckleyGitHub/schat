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

#ifndef INSTALL_H_
#define INSTALL_H_

#include <QProcess>
#include <QQueue>

class QSettings;

class Install : public QObject {
  Q_OBJECT
  
public:
  Install(QObject *parent = 0);
  inline void setClean(bool clean) { m_clean = clean; }
  inline void setRun(bool run)     { m_run = run; }
  
public slots:
  void execute();
  
private slots:
  void fail();
  void finished(int exitCode, QProcess::ExitStatus exitStatus);
  
private:
  void done();
  
  bool m_clean;
  bool m_ready;
  bool m_run;
  QProcess m_process;
  QQueue<QString> m_queue;
  QSettings *m_s;
  QString m_appPath;
  QStringList m_list;
};

#endif /*INSTALL_H_*/
