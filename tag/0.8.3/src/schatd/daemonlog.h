/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DAEMONLOG_H_
#define DAEMONLOG_H_

#include <QFile>
#include <QObject>
#include <QTextStream>

/*!
 * \brief Предоставляет возможность записи в лог.
 */
class DaemonLog : public QObject
{
  Q_OBJECT

public:
  enum Levels {
    Notice,
    Warning,
    Error
  };

  DaemonLog(QObject *parent = 0);
  bool init(const QString &file);
  static DaemonLog *self() { return m_self; }
  void append(const QString &text);
  void append(Levels levels, const QString &text);

private:
  QFile m_file;
  QTextStream m_stream;
  static DaemonLog *m_self;
};

#endif /* DAEMONLOG_H_ */
