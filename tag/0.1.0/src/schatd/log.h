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

#ifndef LOG_H_
#define LOG_H_

#include <QFile>
#include <QObject>
#include <QTextStream>

class Log : public QObject {
  Q_OBJECT
  
public:
  Log(QObject *parent = 0);
  
  bool init();
  void append(const QString &text);
  
private:
  QFile m_file;
  QString m_appPath;
  QTextStream m_stream;
};

#endif /*LOG_H_*/
