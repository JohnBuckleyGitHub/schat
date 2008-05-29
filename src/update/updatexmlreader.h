/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef UPDATEXMLREADER_H_
#define UPDATEXMLREADER_H_

#include <QXmlStreamReader>

class UpdateXmlReader : public QXmlStreamReader {
  
public:
  UpdateXmlReader();
  
  bool readFile(const QString &fileName);
  
private:
  void readCumulative();
  void readFiles();
  void readMeta();
  void readUnknownElement();
  void readUpdates();
};

#endif /*UPDATEXMLREADER_H_*/
