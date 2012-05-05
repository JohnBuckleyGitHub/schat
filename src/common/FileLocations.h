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

#ifndef FILELOCATIONS_H_
#define FILELOCATIONS_H_

#include <QObject>
#include <QHash>

#include "schat.h"

/*!
 * \deprecated Этот класс должен быть заменён на класс Path.
 */
class SCHAT_EXPORT FileLocations : public QObject
{
  Q_OBJECT

public:
  /// Пути к файлам.
  enum Paths {
    AppDirPath, ///< Папка приложения.
    BaseName,   ///< Имя исполняемого файла без расширения.
    ConfigPath, ///< Папка с конфигурационными файлами.
    ConfigFile, ///< Конфигурационный файл.
    SharePath,  ///< Общие файлы.
    VarPath,    ///< Изменяемые файлы.
    LogPath     ///< Папка для журналов.
  };

  /// Схема размещения файлов.
  enum FileScheme {
    Portable,     ///< Стандартное размещение файлов, когда все файлы находятся в папке с исполняемым файлом чата, под Windows схема обеспечивает переносимость без установки.
    UnixAdaptive, ///< Размещение файлов в unix стиле, если исполняемый файл находится в папке bin.
    UnixStandard, ///< Размещение с использованием абсолютных путей если исполняемый файл размещается в /usr/bin.
    UnixDaemon,   ///< Размещение с использованием абсолютных путей типичных для сервера если исполняемый файл размещается в /usr/sbin.
    AppBundle     ///< Размещение специфичное для Mac OS X.
  };

  FileLocations(QObject *parent = 0);
  inline QString path(Paths path) const { return m_paths.value(path); }
  QString file(Paths hint, const QString &name) const;

private:
  QHash<Paths, QString> m_paths;
};

#endif /* FILELOCATIONS_H_ */
