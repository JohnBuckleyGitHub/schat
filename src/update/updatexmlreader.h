/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef UPDATEXMLREADER_H_
#define UPDATEXMLREADER_H_

#include <QQueue>
#include <QStringList>
#include <QXmlStreamReader>

class UpdateXmlReader : public QXmlStreamReader {
  
public:
  UpdateXmlReader();  
  bool isUpdateAvailable() const;
  bool readFile(const QString &fileName);
  inline int coreLevel() const             { return m_coreLevel; }
  inline int qtLevel() const               { return m_qtLevel; }
  inline QString core() const              { return m_core; }
  inline QString qt() const                { return m_qt; }
  inline QQueue<QString> queue() const     { return m_queue; }
  inline void setPath(const QString &path) { m_path = path; }
  
private:
  void readCumulative();
  void readFiles();
  void readMeta();
  void readUnknownElement();
  void readUpdates();
  
  int m_coreLevel;
  int m_qtLevel;
  QQueue<QString> m_queue;
  QString m_core;
  QString m_path;
  QString m_qt;
};

#endif /*UPDATEXMLREADER_H_*/
