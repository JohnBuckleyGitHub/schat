/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef UPDATENOTIFY_H_
#define UPDATENOTIFY_H_

#include <QProcess>

class UpdateNotify : public QObject {
  Q_OBJECT
  
public:
  UpdateNotify(QObject *parent = 0);
  
signals:
  void done(int code);
  
public slots:
  void execute();
  
private slots:
  void error(QProcess::ProcessError error);
  void finished(int exitCode, QProcess::ExitStatus exitStatus);
  
private:
  QString m_appPath;
  QProcess m_process;

};

#endif /*UPDATENOTIFY_H_*/
