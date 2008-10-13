/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA <schat@impomezia.com>
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

#ifndef UPDATEXMLREADER_H_
#define UPDATEXMLREADER_H_

#include <QList>
#include <QStringList>
#include <QXmlStreamReader>

struct FileInfo {
  qint64 size;
  int level;
  QString md5;
  QString type;
  QString name;
};

struct VersionInfo {
  int level;
  QString type;
  QString version;
};

/*!
 * \brief Базовый класс для чтения xml файла обновления.
 */
class UpdateXmlReader : public QXmlStreamReader {

public:
  UpdateXmlReader();
  bool isValid() const;
  bool readFile(const QString &fileName);
  inline QList<FileInfo> files() const          { return m_files; }
  inline QList<VersionInfo> version() const     { return m_version; }
  inline QString platform() const               { return m_platform; }
  inline void platform(const QString &platform) { m_platform = platform; }
  static bool isValid(const FileInfo &file);
  static bool isValid(const VersionInfo &version);

private:
  void readCumulative();
  void readFiles();
  void readMeta();
  void readUnknownElement();
  void readUpdates();

  QList<FileInfo> m_files;
  QList<VersionInfo> m_version;
  QString m_platform;
};

#endif /*UPDATEXMLREADER_H_*/
