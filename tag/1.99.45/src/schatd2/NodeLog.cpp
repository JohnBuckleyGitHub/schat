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

#include <QDateTime>
#include <QDir>
#include <QMutexLocker>

#include "NodeLog.h"

NodeLog::Level NodeLog::m_level = NodeLog::FatalLevel;
NodeLog *NodeLog::m_self = 0;

NodeLog::NodeLog()
{
  m_self = this;
  m_levels += "fatal";
  m_levels += "error";
  m_levels += "warn";
  m_levels += "info";
  m_levels += "debug";
  m_levels += "trace";
}


bool NodeLog::open(const QString &file, Level level)
{
  m_level = level;
  if (level == Disable)
    return false;

  QDir dir(QFileInfo(file).absolutePath());
  if (!dir.exists())
    dir.mkpath(dir.absolutePath());

  bool bom = false;

  m_file.setFileName(file);
  if (!m_file.exists())
    bom = true;

  if (!m_file.open(QFile::WriteOnly | QFile::Text | QFile::Append))
    return false;

  m_stream.setDevice(&m_file);
  m_stream.setGenerateByteOrderMark(bom);
  m_stream.setCodec("UTF-8");

  return true;
}


void NodeLog::add(Level level, const QString &message)
{
  if (level == Disable)
    return;

  QMutexLocker lock(&m_mutex);
  m_stream << time() << " [" << m_levels.at(level) << "] " << message << endl;
}


NodeLog::Helper::~Helper()
{
  write();
}


void NodeLog::Helper::write()
{
  NodeLog::i()->add(m_level, m_buffer);
}


QString NodeLog::time() const
{
  QDateTime dt = QDateTime::currentDateTime();
  QDateTime utc(dt);
  utc.setTimeSpec(Qt::UTC);
  int seconds = dt.secsTo(utc);

  QChar sign = (seconds >= 0 ? '+' : '-');

  if (seconds < 0)
    seconds = -seconds;

  int minutes = (seconds % 3600) / 60;
  int hours = (seconds / 3600);

  QTime t(hours, minutes);
  return dt.toString(Qt::ISODate) + sign + t.toString("hh:mm");
}
