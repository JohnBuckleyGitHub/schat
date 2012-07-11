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

#ifndef NODELOG_H_
#define NODELOG_H_

#include <QDebug>
#include <QFile>
#include <QMutex>
#include <QStringList>
#include <QTextStream>

#include "schat.h"

class SCHAT_EXPORT NodeLog
{
public:
  enum Level {
    Disable = -1,
    FatalLevel,
    ErrorLevel,
    WarnLevel,
    InfoLevel,
    DebugLevel,
    TraceLevel
  };

  NodeLog();
  bool open(const QString &file, Level level);
  inline static Level level() { return m_level; }
  inline static NodeLog *i()  { return m_self; }
  void add(Level level, const QString &message);

  class SCHAT_EXPORT Helper
  {
  public:
    explicit Helper(Level level)
    : m_level(level)
    , m_debug(&m_buffer)
    {}

    ~Helper();

    inline QDebug& stream() { return m_debug; }

  private:
    void write();

    Level m_level;
    QDebug m_debug;
    QString m_buffer;
  };

private:
  QString time() const;

  QFile m_file;           ///< Файл журнала.
  QMutex m_mutex;         ///< Мьютекс защищающий запись.
  QStringList m_levels;   ///< Текстовые обозначения уровней журналирования.
  QTextStream m_stream;   ///< Текстовый поток для записи в журнал.
  static Level m_level;   ///< Уровень ведения журнала.
  static NodeLog *m_self; ///< Указатель на себя.
};


#define SCHAT_LOG_FATAL(x)  \
  if (NodeLog::level() < NodeLog::FatalLevel) {} \
  else NodeLog::Helper(NodeLog::FatalLevel).stream() << x ;
#define SCHAT_LOG_ERROR(x)  \
  if (NodeLog::level() < NodeLog::ErrorLevel) {} \
  else NodeLog::Helper(NodeLog::ErrorLevel).stream() << x ;
#define SCHAT_LOG_WARN(x)  \
  if (NodeLog::level() < NodeLog::WarnLevel) {} \
  else NodeLog::Helper(NodeLog::WarnLevel).stream() << x ;
#define SCHAT_LOG_INFO(x)  \
  if (NodeLog::level() < NodeLog::InfoLevel) {} \
  else NodeLog::Helper(NodeLog::InfoLevel).stream() << x ;
# define SCHAT_LOG_DEBUG(x)  \
  if (NodeLog::level() < NodeLog::DebugLevel) {} \
  else NodeLog::Helper(NodeLog::DebugLevel).stream() << x ;
# define SCHAT_LOG_TRACE(x)  \
  if (NodeLog::level() < NodeLog::TraceLevel) {} \
  else NodeLog::Helper(NodeLog::TraceLevel).stream() << x ;


#endif /* NODELOG_H_ */
