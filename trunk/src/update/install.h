/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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
