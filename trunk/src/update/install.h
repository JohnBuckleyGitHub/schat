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
  
public slots:
  void execute();
  
private slots:
  void fail();
  void finished(int exitCode, QProcess::ExitStatus exitStatus);
  
private:
  bool m_ready;
  QProcess m_process;
  QQueue<QString> m_queue;
  QSettings *m_s;
};

#endif /*INSTALL_H_*/
