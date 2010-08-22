/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 - 2009 IMPOMEZIA <schat@impomezia.com>
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

#ifndef UPDATEXMLREADER_H_
#define UPDATEXMLREADER_H_

#include <QList>
#include <QMultiMap>
#include <QStringList>
#include <QXmlStreamReader>

#ifndef SCHAT_NO_UPDATE
struct FileInfo {
  qint64 size;
  int level;
  QString md5;
  QString type;
  QString name;
};
#endif

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
  inline QList<VersionInfo> version() const       { return m_version; }
  static bool isValid(const VersionInfo &version);
  #ifndef SCHAT_NO_UPDATE
    inline QMultiMap<int, FileInfo> files() const { return m_files; }
    inline QString baseUrl() const                { return m_baseUrl; }
    static bool isValid(const FileInfo &file);
  #endif

private:
  void readMeta();
  void readUnknownElement();
  void readUpdates();
  #ifndef SCHAT_NO_UPDATE
    void readCumulative();
    void readFiles();
  #endif

  QList<VersionInfo> m_version;

  #ifndef SCHAT_NO_UPDATE
    QString m_baseUrl;
    QMultiMap<int, FileInfo> m_files;
  #endif
};

#endif /*UPDATEXMLREADER_H_*/
