/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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
