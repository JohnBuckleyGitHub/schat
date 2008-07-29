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

#ifndef UPDATEXMLREADER_H_
#define UPDATEXMLREADER_H_

#include <QList>
#include <QStringList>
#include <QXmlStreamReader>

struct FileInfo {
  QString name;
  qint64 size;
  QString md5;
  QString type;
  int level;
};

class UpdateXmlReader : public QXmlStreamReader {
  
public:
  UpdateXmlReader();  
  bool isUpdateAvailable() const;
  bool readFile(const QString &fileName);
  inline int coreLevel() const             { return m_coreLevel; }
  inline int qtLevel() const               { return m_qtLevel; }
  inline QString core() const              { return m_core; }
  inline QString qt() const                { return m_qt; }
  inline QList<FileInfo> list() const      { return m_list; }
  
private:
  void readCumulative();
  void readFiles();
  void readMeta();
  void readUnknownElement();
  void readUpdates();
  
  int m_coreLevel;
  int m_qtLevel;
  QList<FileInfo> m_list;
  QString m_core;
  QString m_qt;
};

#endif /*UPDATEXMLREADER_H_*/
