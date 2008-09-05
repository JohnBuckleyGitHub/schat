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

#ifndef UPDATENOTIFY_H_
#define UPDATENOTIFY_H_

#include <QProcess>

class Settings;

class UpdateNotify : public QObject {
  Q_OBJECT
  
public:
  UpdateNotify(Settings *settings, QObject *parent = 0);
  
signals:
  void done(int code);
  
public slots:
  void execute();
  
private slots:
  void error(QProcess::ProcessError error);
  void finished(int exitCode, QProcess::ExitStatus exitStatus);
  
private:
  QProcess m_process;
  QString m_appPath;
  Settings *m_settings;
};

#endif /*UPDATENOTIFY_H_*/
