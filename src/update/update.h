/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 */

#ifndef UPDATE_H_
#define UPDATE_H_

#include <QObject>

#include "updatexmlreader.h"

class Download;

static const QString DefaultUpdateXmlUrl = "http://achim.net/schat/updates/update.xml";

class Update : public QObject {
  Q_OBJECT
  
public:
  enum State {
    GettingUpdateXml,
    ReadingUpdateXml,
    GettingUpdates
  };
  
  Update(QObject *parent = 0);
  
public slots:
  void execute();
  
private slots:
  void saved(const QString &filename);
  
private:
  Download *m_download;
  State m_state;
  UpdateXmlReader m_reader;
};

#endif /*UPDATE_H_*/
