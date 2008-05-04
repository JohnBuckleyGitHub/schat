/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef NETWORKREADER_H_
#define NETWORKREADER_H_

#include <QXmlStreamReader>

class NetworkReader : public QXmlStreamReader {
  
public:
  NetworkReader();
  
  bool readFile(const QString &fileName);
  inline QString description() const { return m_description; }
  inline QString networkName() const { return m_name; }
  inline QString site() const        { return m_site; }
  inline QStringList servers() const { return m_servers; }
  
private:
  void readMeta();
  void readNetwork();
  void readServers();
  void readUnknownElement();
  
  QString m_description;
  QString m_name;
  QString m_site;
  QStringList m_servers;
};

#endif /*NETWORKREADER_H_*/
